#!/usr/bin/env bash
# UEFI smoke test: boot the built disable-ram-area.efi under QEMU + OVMF and
# fail if it triggers a CPU exception (e.g. a NULL BS dereference from a missing
# InitializeLib). Run with no LoadOptions, the program calls BS->HandleProtocol
# and Print() and then returns "No command line arguments provided." — so a
# clean run exercises the gnu-efi library initialization. A broken build instead
# faults with an invalid-opcode / page-fault before printing anything.
#
# Usage: scripts/uefi-smoke-test.sh path/to/disable-ram-area.efi
set -euo pipefail

EFI="${1:-disable-ram-area.efi}"
[ -f "$EFI" ] || { echo "smoke-test: EFI not found: $EFI" >&2; exit 2; }

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# Locate OVMF firmware (Debian/Ubuntu ship 4M split files; fall back to others).
OVMF_CODE=""
for c in /usr/share/OVMF/OVMF_CODE_4M.fd /usr/share/OVMF/OVMF_CODE.fd \
         /usr/share/ovmf/OVMF.fd /usr/share/qemu/OVMF.fd; do
  [ -f "$c" ] && OVMF_CODE="$c" && break
done
[ -n "$OVMF_CODE" ] || { echo "smoke-test: OVMF firmware not found" >&2; exit 2; }
OVMF_VARS="${OVMF_CODE/CODE/VARS}"
[ -f "$OVMF_VARS" ] || OVMF_VARS="/usr/share/OVMF/OVMF_VARS_4M.fd"
cp "$OVMF_VARS" "$WORK/vars.fd"

# Build a tiny FAT ESP with the app at the removable-media default path.
dd if=/dev/zero of="$WORK/esp.img" bs=1M count=48 status=none
mkfs.vfat "$WORK/esp.img" >/dev/null
mmd -i "$WORK/esp.img" ::/EFI ::/EFI/BOOT
mcopy -i "$WORK/esp.img" "$EFI" ::/EFI/BOOT/BOOTX64.EFI

LOG="$WORK/qemu-int.log"
echo "smoke-test: booting $EFI under OVMF ($OVMF_CODE) ..."
timeout 60 qemu-system-x86_64 \
  -machine q35 -m 256 \
  -drive if=pflash,format=raw,unit=0,readonly=on,file="$OVMF_CODE" \
  -drive if=pflash,format=raw,unit=1,file="$WORK/vars.fd" \
  -drive format=raw,file="$WORK/esp.img" \
  -display none -serial null \
  -no-reboot -d int,cpu_reset -D "$LOG" >/dev/null 2>&1 || true

# CPU exception vectors are 0x00-0x1f; a clean OVMF boot logs none of these.
# check_exception / "Triple fault" also indicate a fault.
if grep -aE "check_exception|Triple fault|v=0[0-9a-f] |v=1[0-9a-f] " "$LOG" >/dev/null 2>&1; then
  echo "smoke-test: FAIL — binary caused a CPU exception (likely missing InitializeLib):" >&2
  grep -aE "check_exception|Triple fault|v=0[0-9a-f] |v=1[0-9a-f] " "$LOG" | head -3 >&2
  exit 1
fi

echo "smoke-test: PASS — no CPU exception; binary initialized and ran cleanly."
