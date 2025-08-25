# EFI Memory Patch Utility for macOS (Intel CPUs)

This utility disables defective memory regions on Intel-based Macs to prevent system crashes or kernel panics caused by faulty RAM. It is not compatible with Apple Silicon Macs. 

Chances are high, you can resolve the following error when executing this utility (automatically) before each macOS Boot:

- **PPM002, PPM003, PPM004...**: _"There may be an issue with the onboard memory."_

This tool fulfills the same purpose as the [grub-badram](https://www.gnu.org/software/grub/manual/grub/html_node/badram.html) command which disables RAM-Areas on Linux Systems.

## Requirements
- Intel-based Mac or MacBook
- Diagnostic tool like [Memtest86+](https://www.memtest.org/) to identify faulty memory addresses
- EFI boot manager like [rEFInd](https://www.rodsbooks.com/refind/)

## Usage
1. **Identify Faulty Memory**: Run Memtest86+ in Error-Summary-Mode to note the lowest and highest addresses of defective RAM.
2. **Run Utility**: Use rEFInd to execute this EFI utility before booting macOS. It marks faulty memory as unusable.
3. **Boot macOS**: Start macOS normally.
4. **Verify**: Check if memory is disabled by running:
   ```bash
   sysctl -a | grep mem | grep Unusable
   ```
   Expected output: `machdep.memmap.Unusable: <size_of_disabled_area>`
5. **Automate**: Follow [this guide](https://github.com/0nelight/macOS-Disable-RAM-Areas/discussions/3) to automate the process.

## Alternative
Use [OpenCore Legacy Patcher](https://github.com/0nelight/macOS-Disable-RAM-Areas/discussions/14) to configure defective RAM areas. Verify compatibility [here](https://github.com/0nelight/macOS-Disable-RAM-Areas/discussions/18).

## Compatibility
Tested on:
- MacBook Pro (Late 2013, macOS Big Sur)
- MacBook Pro (2019, 16") ([see discussion](https://github.com/0nelight/macOS-Disable-RAM-Areas/discussions/3#discussioncomment-10753593))
- MacBook Pro (2020, macOS Sequoia) ([see discussion](https://github.com/0nelight/macOS-Disable-RAM-Areas/discussions/18#discussion-8795338))
- Additional success stories on the [Issues page](https://github.com/0nelight/macOS-Disable-RAM-Areas/issues?q=label%3Asuccess+is%3Aclosed)

## Support
- Read the [Release Notes](https://github.com/0nelight/macOS-Disable-RAM-Areas/releases/tag/v0.2.1-beta) and [Discussions](https://github.com/0nelight/macOS-Disable-RAM-Areas/discussions).
- For questions, open a [new discussion](https://github.com/0nelight/macOS-Disable-RAM-Areas/discussions/new?category=q-a).
- Share success stories [here](https://github.com/0nelight/macOS-Disable-RAM-Areas/issues/new?assignees=&labels=success&projects=&template=success-story.md&title=Success-Story).

## Disclaimer
This utility is not affiliated with Apple Inc. Use at your own risk. The author is not responsible for any outcomes or time invested. Provided as-is, without warranties, for personal use only.

## Some additonal Notes:

### Screenshot-from refind (instead of grubx64.efi you can have your DISABLE-RAM.EFI utility there):
![refind](https://github.com/user-attachments/assets/86a70d90-0975-4411-bf5b-49966757245d)

### The Place of this Utility within the Boot Process of a Mac:
![grafik](https://github.com/user-attachments/assets/2ee9237c-1e67-4339-aa63-3c1d718bb8ba)

### Documentation of AllocatePages-Function:
Documentation of this function: `gBS -> AllocatePages(2, 8, 40960, & Addr);` [efi-v1-10-specification.pdf](https://github.com/user-attachments/files/16910413/efi-v1-10-specification.pdf) Page: 119

### More Information on Mac-Internals:
https://eclecticlight.co/mac-troubleshooting-summary/

