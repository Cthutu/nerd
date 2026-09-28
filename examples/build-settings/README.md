# Source build settings

Run `just run-example build-settings` from the repository root.

The source selects a console executable and enables its own `development`
feature. That define affects this module, including function-level `on` blocks;
it does not change the definitions seen by `std.io`.

Native dependency modules can add `library_path` entries in their own build
blocks. Importers inherit these paths automatically and search their own paths
first. See [configuration](../../docs/configuration.md) for the complete syntax.
