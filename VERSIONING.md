# Versioning
This project uses Semantic Versioning. As with many projects, Major implies non-compatible changes, Minor implies compatible changes, and Patch implies bug fixes, optimizations, and the like.

Additionally, an optional non-standard field, prefixed with `+`, indicates a revision made to the codebase that does not result in any meaningful changes to the resulting binary or binaries.
>```
>v1.2.3+a
>```
In this case, the `a` indicates such a change. The binary may be slightly different due to code refactoring (for example), but its function remains the same, making the revision only meaningful on the development side of things.

The revision field behaves similarly to decimal, but with lowercased Latin letters. `v1.2.3+aa` would indicate a lengthy refactoring process of some kind without it affecting the program's output. This example is unlikely, but technically possible.

In short:
>```
>vMajor.minor.patch+revision
>```
