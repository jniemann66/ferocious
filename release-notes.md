## What's New in v2.2.0

### New Features

- **ReSampler capability detection** — ferocious now queries the installed ReSampler binary at startup to determine what features and formats it supports (including whether MP3 encoding is available via libsndfile).
- **Info panel in Input File Selection Dialog** — the file picker now displays an info panel with details about selected audio files.
- **Audio preview in File Dialog** — sound files can be played/previewed directly from the file selection dialog.
- **Native vs. non-native file dialogs** — a new option lets you choose between the OS-native file dialogs and ferocious's own dialogs. Non-native mode is now the default.
- **Output format selector in main window** — a new output format combo box now sits directly to the left of the Browse Output File button, making it much more intuitive to set the output format without having to open a separate dialog. The Output File Format dialog itself has also been improved with a combo box selector.
- **Overhauled theming system** — themes are now built into the application as Qt resources and loaded dynamically, with a new Theme Selection dialog for browsing and switching between them.
- **High DPI scaling** — improved rendering on high-resolution displays.
- **Double-precision as default** — the default processing precision is now double.
- **macOS: ReSampler location prompt** — a descriptive message box now appears before prompting the user to locate the ReSampler binary (macOS doesn't show a title in the file location dialog).

### UI Improvements

- More pronounced highlighting of selected items in non-native file dialogs.
- Various UI consistency improvements across dialogs.
- CSS and theme refinements, including a less intense "pressed" color in the flat theme.
- Fixed saving and restoring of window geometry between sessions.

### Bug Fixes

- Fixed crash when the ReSampler version query returns an empty or unexpected result.
- Fixed issue where the saved theme was not being applied at startup.
- Fixed `ConverterDefinitionsModel` not notifying the view when its underlying data was updated.
- Fixed launching of LAME on Windows.
- Completely revised command-line assembly and process launching for converters — resolves issues on Windows and improves reliability on Linux.
- Fixed formatting of newline characters from converter output.

### Build & CI

- Added GitHub Actions workflow for Linux builds, packaging ferocious as an **AppImage** on Ubuntu 22.04 using Qt 6.10.1.
- Added GitHub Actions workflow for **Windows builds**.
