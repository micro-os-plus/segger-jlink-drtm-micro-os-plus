[![license](https://img.shields.io/github/license/micro-os-plus/segger-jlink-drtm-micro-os-plus)](https://github.com/micro-os-plus/segger-jlink-drtm-micro-os-plus/blob/xpack/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/micro-os-plus/segger-jlink-drtm-micro-os-plus.svg)](https://github.com/micro-os-plus/segger-jlink-drtm-micro-os-plus/issues)
[![GitHub pulls](https://img.shields.io/github/issues-pr/micro-os-plus/segger-jlink-drtm-micro-os-plus.svg)](https://github.com/micro-os-plus/segger-jlink-drtm-micro-os-plus/pulls)

# SEGGER J-Link GDB Server plug-in for µOS++ IIIe

A source xPack with the µOS++ thread-aware plug-in for the SEGGER J-Link GDB Server.

This plug-in shows all existing threads in a µOS++ IIIe application.

## Easy install

```console
$ npm install @ilg/segger-jlink-drtm-micro-os-plus
```

## Original files

This xPack does not include any original SEGGER files.

## License

The original content is released under the
[MIT License](https://opensource.org/licenses/MIT), with all rights reserved to
[Liviu Ionescu](https://github.com/ilg-ul).

The plug-in client API definitions (the `RTOS_*` functions) and the 
GDB server API definitions are compatible with the SEGGER GDBServer 
RTOS Plug-in SDK API definitions.

All IP rights, title and interest in the GDBServer RTOS Plug-in SDK
are and shall at all times remain with SEGGER.

```
Copyright (c) 2004-2016 SEGGER Microcontroller GmbH & Co. KG
Internet: www.segger.com        Support:  support@segger.com
```
