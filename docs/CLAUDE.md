# CLAUDE.md: tinykv

## What this project is

tinykv is a durable single-node key-value store in C11 for Linux: an append-only write-ahead log split into segments, an in-memory hash index mapping keys to log offsets, crash recovery with torn-write handling, background log compaction, and a single-threaded `epoll` TCP server speaking a subset of the Redis protocol (RESP2).

**`docs/SPEC.md` is the source of truth** for formats, commands, behaviour, testing, and milestones. Read it before starting any work. If something you're about to do conflicts with the spec, stop and ask rather than silently deviating. If the spec is ambiguous or wrong, propose a change to the spec and wait for approval.

## Who you're working with

I'm building this to learn storage and systems engineering and to discuss it confidently in interviews. That shapes how you should work:

- **Explain as you go.** When you make a non-obvious decision (probe strategy, buffer ownership, fsync placement, lock scope), say why in a short comment in the code or in `docs/DESIGN.md`, not only in chat.
- **Flag interview-worthy concepts.** When something touches durability, crash consistency, or concurrency, add a short note to `docs/DESIGN.md` under the relevant section so I can study it later.
- **Don't hide complexity.** Prefer clear, explicit code over clever code. I need to be able to read and explain every file.
- If I ask you to let me write a part myself, give me the function signature, the tests it must pass, and hints, but not the implementation.

## How we work: one milestone at a time

The milestones are in `docs/SPEC.md` §8. For each one:

1. **Plan first.** Before writing code, post a short plan: files you'll create or change, public function signatures, data structures, and the tests you'll write. Wait for my go-ahead.
2. **Tests alongside code.** Write unit tests in the same milestone as the code they test. Don't defer testing.
3. **Verify.** Build with warnings-as-errors, run the full test suite, and run the new tests under Valgrind (and ASan/UBSan once that build option exists). Report the actual results, including failures.
4. **Summarize.** At the end, update `docs/PROGRESS.md` with: what was done, what was tested, known limitations, and anything deferred. Then give me a short summary in chat.
5. **Commit.** Make small, logical commits with clear messages (e.g. `record: add CRC-32C validation to decoder`). Don't bundle unrelated changes.
6. **Stop.** Do not start the next milestone until I say so.

## Build and test commands

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure

# Sanitizers (once added in milestone 11, or earlier if useful)
cmake -B build-asan -DTINYKV_SANITIZE=ON && cmake --build build-asan -j
ctest --test-dir build-asan --output-on-failure

# Valgrind on a unit test binary
valgrind --leak-check=full --error-exitcode=1 ./build/tests/unit/test_<module>
```

Keep these commands working. If you change how the build works, update this section.

## Coding standards

- C11 with POSIX.1-2008 plus Linux `epoll`, `signalfd`, `timerfd`. No other platforms.
- Compile cleanly with `-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror` on both GCC and Clang.
- **No runtime dependencies.** Unity is the only third-party code, for tests only, fetched with CMake `FetchContent` or vendored under `third_party/`.
- Format with the committed `.clang-format`. Run it before every commit.
- Module prefixes on public symbols: `tkv_record_*`, `tkv_index_*`, `tkv_wal_*`, `tkv_engine_*`, `tkv_resp_*`, etc. `static` for everything not in a header.
- Functions return an `int` status (0 on success, negative error code from a shared `tkv_err` enum) and use out-parameters for results. No `exit()` or `abort()` outside `main.c` and the debug fault-injection hooks.
- Every allocation has a clear owner. Document ownership in the header comment when a function returns or takes ownership of memory.
- Fixed-width integer types (`uint32_t`, etc.) for anything on disk or on the wire. Explicit little-endian encode/decode helpers; never `memcpy` a struct to disk.
- The storage engine (`engine`, `wal`, `record`, `index`, `recovery`, `compaction`) must not depend on the network layer. It must be usable as a plain library.

## Correctness rules (never break these)

- **Never reply `+OK` to a write before it is durable** according to the active `--fsync` mode.
- **Check every syscall return value.** Retry on `EINTR`. Loop on short `write()`/`pwrite()`/`read()`.
- **`fsync` the directory** after creating, renaming, or deleting any file in the data directory.
- **Validate before allocating.** Decoders check lengths against configured limits and remaining file size before any `malloc`. A corrupted length field must never cause a huge allocation.
- **A torn tail in the last segment is truncated; corruption in an earlier segment is a hard error** (unless `--repair`). Don't blur these.
- **Compaction must only update an index entry if it still points at the old location.**
- **Never weaken, skip, or delete a test to make it pass.** If a test seems wrong, explain why and ask.
- **Never change the on-disk format** without updating `docs/SPEC.md` and bumping the format version, with my approval.

## Testing expectations

- Unit tests with Unity live in `tests/unit/`, one file per module.
- Test edge cases deliberately: empty values, max sizes, inputs split at every byte boundary, corrupted bytes at every offset where practical.
- Integration tests in `tests/integration/` (Python, stdlib only) start the real binary on a random free port with a temp data dir and clean up after themselves.
- Crash tests in `tests/crash/` follow SPEC §5.3. Fault injection hooks are compiled only in debug builds (`#ifdef TKV_FAULT_INJECTION`) and controlled by environment variables.
- Tests must be deterministic. If randomness is used, log the seed and allow it to be set via an environment variable so failures can be reproduced.

## Documentation to maintain

- `docs/SPEC.md`: requirements. Change only with approval.
- `docs/DESIGN.md`: how and why things work. Update it in every milestone that makes a design decision.
- `docs/PROGRESS.md`: milestone log and known limitations.
- `README.md`: written properly in milestone 11, but keep the quick-start accurate from milestone 7 onwards.
- `BENCHMARKS.md`: only real, reproducible numbers with hardware, mode, and exact command. Never invent or estimate figures.

## Things not to do

- Don't add features outside the spec, even small ones, without asking.
- Don't introduce threads anywhere except the compaction thread.
- Don't use `mmap` for the log (the spec uses `pread`/`pwrite`); ask if you think it's warranted.
- Don't claim something works without having run it. If you couldn't run a test, say so.
