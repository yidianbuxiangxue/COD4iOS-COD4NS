#!/bin/bash
set -euo pipefail

ipa="${1:?usage: audit_ipa.sh path/to/app.ipa}"
expected_minos="${KISAK_IOS_MIN_VERSION:-16.1}"
work_dir="$(mktemp -d "${TMPDIR:-/tmp}/cod4ios-ipa-audit.XXXXXX")"
cleanup() { rm -rf -- "$work_dir"; }
trap cleanup EXIT

ditto -x -k "$ipa" "$work_dir"
app="$(find "$work_dir/Payload" -maxdepth 1 -type d -name '*.app' -print -quit)"
[[ -n "$app" ]] || { echo "No app bundle in $ipa" >&2; exit 1; }

plist="$app/Info.plist"
plist_minos="$(/usr/libexec/PlistBuddy -c 'Print :MinimumOSVersion' "$plist")"
[[ "$plist_minos" == "$expected_minos" ]] || {
    echo "Info.plist MinimumOSVersion is $plist_minos, expected $expected_minos" >&2
    exit 1
}

executables=(
    "$app/KisakCOD"
    "$app/Frameworks/libkisakcod_sp.dylib"
    "$app/Frameworks/libkisakcod_mp.dylib"
)

for binary in "${executables[@]}"; do
    [[ -f "$binary" ]] || { echo "Missing executable: $binary" >&2; exit 1; }
    echo "===== ${binary#"$app/"} ====="

    archs="$(lipo -archs "$binary")"
    echo "architectures: $archs"
    [[ "$archs" == "arm64" ]] || {
        echo "Unexpected architecture set for $binary: $archs" >&2
        exit 1
    }

    build_version="$(xcrun vtool -show-build "$binary")"
    printf '%s\n' "$build_version"
    grep -Eq "minos[[:space:]]+$expected_minos([[:space:]]|$)" <<<"$build_version" || {
        echo "Mach-O minOS is not $expected_minos: $binary" >&2
        exit 1
    }

    otool -L "$binary"
    imports="$work_dir/$(basename "$binary").imports"
    xcrun nm -u "$binary" > "$imports"
    if grep -E '__ZNSt3__18(to|from)_charsEPcS0_[def]' "$imports"; then
        echo "Forbidden iOS 16.3+ floating-point charconv import in $binary" >&2
        exit 1
    fi
done

echo "IPA audit passed: arm64, minOS $expected_minos, required engines present, no floating charconv imports."
