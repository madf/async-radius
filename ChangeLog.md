# ChangeLog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2024-11-12
### Added.
 - Initial release.

## [1.1.1] - 2025-02-19
### Added.
 - `Socket::close` method.

## [1.1.2] - 2025-04-11
### Changed.
 - Sample server checks user name name.

### Fixed.
 - Message authenticator in some response packets.
 - Uninitialized class members.

### Added.
 - A possibility to use the project as a git submodule.

## [1.1.3] - 2025-07-07
### Fixed.
 - Boost-1.88 compatibility (#57).
 - Deprecated OpenSSL functions (#59).
 - Deprecated Boost.ASIO functions (#61).

## [1.2.0] - 2026-01-14
### Added.
 - Attribute types in dictionaries.
 - `Attribute::make` constructor function.
