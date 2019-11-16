# Virgil IoTKit C

[![Build Status](https://travis-ci.com/VirgilSecurity/virgil-iot-sdk.svg?branch=master)](https://travis-ci.com/VirgilSecurity/virgil-iot-sdk)
[![Documentation Doxygen](https://img.shields.io/badge/docs-doxygen-blue.svg)](http://VirgilSecurity.github.io/virgil-iot-sdk)
[![GitHub license](https://img.shields.io/badge/license-BSD%203--Clause-blue.svg)](https://raw.githubusercontent.com/VirgilSecurity/virgil-iot-sdk/release/LICENSE)


## Introduction

<a href="https://developer.virgilsecurity.com/docs"><img width="230px" src="https://cdn.virgilsecurity.com/assets/images/github/logos/virgil-logo-red.png" align="left" hspace="10" vspace="6"></a>[Virgil Security](https://virgilsecurity.com) provides a set of APIs for adding security to any application and devices.

Virgil IoTKit is a C library for connecting IoT devices to Virgil IoT Security PaaS. IoTKit helps you easily add security to your IoT devices at any lifecycle stage for secure provisioning and authenticating devices, secure updating firmware and trust chain, and for secure exchanging messages using any transport protocols.

## Content
- [Features](#features)
- [IoT Dev Tools](#iot-dev-tools)
- [Run IoTKit Demo](#run-iotkit-demo)
- [IoTKit Modules](#modules)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Ubuntu, Debian OS](#ubuntu-debian-os)
  - [CentOS, Fedora OS](#centos-fedora-os)
  - [MacOS](#macos)
  - [Windows OS](#windows-os)
- [Tests](#tests)
- [API Reference](#api-reference)
- [License](#license)
- [Support](#support)

## Features
Virgil IoTKit provides a set of features for IoT device security and management:
- **Crypto Module. Connect any crypto library and hard secure module (HSM)**. Virgil IoTKit provides flexible and simple API for any types of crypto library and HSM. At the same time, the framework provides default software HSM implementation based on Virgil Crypto. (Support for ATECC608A and ATECC508A in the next version).
- **Provision Module. Secure IoT device provision**. In order to securely update firmware, securely register, authenticate or exchange messages between IoT devices, each IoT device must have its own tools, which allow the device to perform cryptographic operations. These tools must contain the necessary information to identify the device or other participants (e.g., trust list provider). These tools are the device keys, trust chain, device card, etc. The process of providing your IoT devices with these tools is called device provision, and Virgil IoTKit gives you all the necessary functionality to make your IoT devices identifiable and verifiable, as a result, protect them from counterfeit and fabrication.
- **Firmware Module. Secure firmware and trust chain verification and distribution**. IoTKit provides a set of API for secure verification of firmware in a bootloader. Also, IoTKit provides an example of bootloader implementation in the form of emulator.
- **Secbox Module. Secure software storage for any sensitive data**. IoTKit provides secure software storage that is called **Secbox** for storing any sensitive data, like private keys. The Secbox works in two modes; the first mode - when data is only signed, and the second one - when data is encrypted and then signed.
- **Protocols Module**. IoTKit provides a flexible, programmable and security network adaptive protocol (SNAP) for device-to-device, device-to-cloud, and cloud-to-device communication. SNAP can be used for secure firmware distribution, secure notification about device state, secure device provision. Also, SNAP contains a set of functions and interfaces that allows you to work with any transport protocol (BLE, Wi-Fi, PLC, NoiseSocket, etc.).  
- **Cloud Module. API for working with Virgil IoT Security PaaS**. IoTKit interacts with the Virgil IoT Security Platform as a Service (PaaS) to provide you with the services for security, management, and monitoring IoT devices.
- **Logger Module**. IoTKit contains a set of functions and interfaces for logging device events.

## IoT Dev Tools
Virgil Security also provides set of tools for secure device lifecycle.
- **Virgil Trust Provisioner**. The Virgil Trust Provisioner is a CLI used to manage your distributed trust between all parties, including IoT devices, in your IoT solutions. The CLI is aimed at key pairs and TrustLists generation and management, which together make each IoT device identifiable, verifiable and trusted by each party of IoT solution. To start working with the tool read more [here](/tools/virgil-trust-provisioner).
- **Virgil Device Initializer**. In order to make each IoT device identifiable, verifiable and trusted by each party of IoT solution you have to provide it with specific provision files, generate private keys and create the digital cards for further device registration in Cloud. Virgil Device Initializer allows you to make IoT device provisioning and prepare your IoT device (create digital cards) for its further registration in Virgil Cloud. To start working with the tool read more [here](/tools/virgil-device-initializer).
- **Virgil Device Registrar**. Virgil IoT Device Registrar is used to registrar IoT devices and their digital cards in the Virgil Security Cloud. To start working with the tool read more [here](/tools/virgil-device-registrar).
- **Virgil Firmware Signer**. Virgil Firmware Signer is a CLI that allows you to sign a firmware using Auth and Firmware Private Keys to provide firmware integrity before distributing it. To start working with the tool read more [here](/tools/virgil-firmware-signer).
- **Virgil SnapD**. Virgil SnapD is a local web utility which allows you to obtain information and statistics of your IoT devices. In order to get such device information SnapD interacts with Virgil SNAP protocol, which operates directly with your IoT devices. As far as Virgil SnapD is a local service, the obtained information can be displayed in browser under http://localhost:8080/ (by default). In case you work with Virgil IoT Simulator, you can run SnapD under http://localhost:8081/. To start working with the tool read more [here](/tools/virgil-snapd).


## Run IoTKit Demo
To demonstrate our IoTKit in action we developed [Demo for NIX systems based on IoTKit toolkits](https://github.com/VirgilSecurity/demo-iotkit-nix).

The IoTKit Demo is conditionally divided into 3 actors (Vendor, Factory and End-user) and shows secure lifecycle of IoT devices. The IoTKit Demo allows you to:
- **Generate trusted provisioning package**. To start working with emulated IoT infrastructure the Demo uses Virgil Trust Provisioner utility for generating provisioning files, such as private keys (e.g. for factory, firmware) and a distributed trust list that contains public keys and signatures of trusted services providers (e.g. factory, cloud).
- **Emulate IoT devices**. Then, you can emulate two IoT device types: IoT Gateway - an internet-capable smart device that communicates with other IoT devices and Clouds; and IoT Device - end-device, like smart bulb, that can be controlled remotely through the IoT Gateway.
- **Securely perform IoT device provisioning**. Demo uses the Virgil Device Initializer for IoT devices provisioning to make them identifiable, verifiable and trusted. Securely integrate trust list and crypto library on IoT devices, then generate key pairs and create digital cards, and sign digital cards with the Factory Key.
- **Register IoT devices on the security platform**. At this step the Virgil Device Registrar is used to register digital cards of IoT devices at Virgil Cloud for further device authentication and management.
- **Sign and publish new Firmware and TrustList**. Also, you can emulate process of creating and publishing new Firmware or TrustList to Virgil Cloud. Demo uses Virgil Firmware Signer to sign a firmware before its distributing.
- **Manage IoT devices**. Demo allows to manage IoT devices and get information about their state. Demo uses Virgil services to notify IoT devices about new updates and then securely verify incoming firmware or trust lists before updating them.

<img width="100%" src="https://cdn.virgilsecurity.com/assets/images/github/virgil_demo_iotkit_nix.png" align="left" hspace="0" vspace="6"> &nbsp;

To start working with the Demo head over to [Demo IoTKit Nix GitHub](https://github.com/VirgilSecurity/demo-iotkit-nix) repository and follow the instructions in the README.


## IoTKit Modules
As we mentioned above, Virgil IoTKit provides a set of features that implemented to modules:
- **Crypto Module** is used for cryptographic operations with callbacks for [Hardware Security Modules supports](https://virgilsecurity.github.io/virgil-iotkit/cloud_8h.html) and [cryptographic converters](https://virgilsecurity.github.io/virgil-iotkit/crypto__format__converters_8h.html).
- **[Cloud Module](https://virgilsecurity.github.io/virgil-iotkit/cloud_8h.html)** is used for for obtaining credentials from Virgil Thing service and downloading firmware images and trustlist files from cloud storage. .
- **[Firmware Module](https://virgilsecurity.github.io/virgil-iotkit/firmware_8h.html)** is used for firmware downloading, uploading and processing by IoT Gateway or by Thing (IoT Device).
- Provision.
- Secbox.
- Protocols.
- Cloud.
- Logger.

## Installation
Virgil IoTKit is distributed as a package. This section demonstrates on how to install Virgil IoTKit for preferred platform.

### Prerequisites
To start working with Virgil IoTKit the following components are required:
- CMake v3.11 or higher, for project building
- GCC or another toolchain for C/C++ compile
- [Go](https://golang.org/) library to work with Virgil IoT utilities
- [git](https://git-scm.com/) for Virgil Crypto installation and update
- [curl](https://curl.haxx.se/)


### Ubuntu, Debian OS
To download and install the Virgil IoTKit on Ubuntu, use the following command:

```shell
$ apt install make gcc cmake golang git libcurl4-openssl-dev doxygen swig
```

### Fedora, CentOS
To download and install the Virgil IoTKit on Fedora or CentOS, use the following command:

```shell
$ yum install make cmake golang git gcc gcc-c++ libcurl-devel doxygen swig
```

### MacOS
To download and install the Virgil IoTKit on MacOS, use the following command:
```shell
$ brew install make cmake golang git gcc curl doxygen swig
```

- Check CMake version. It must be 3.11 or higher:

```shell
$ cmake --version
cmake version 3.11.0
```

### Windows OS
Virgil IoTKit for Windows OS is currently in development. To be included to information update list please contact our support team: support@VirgilSecurity.com.


## Tests

## API Reference
- [API Reference of IoTKit](http://VirgilSecurity.github.io/virgil-iot-sdk)

## License

This library is released under the [3-clause BSD License](LICENSE).

## Support
Our developer support team is here to help you. Find more information on our [Help Center](https://help.virgilsecurity.com/).

You can find us on [Twitter](https://twitter.com/VirgilSecurity) or send us email support@VirgilSecurity.com.

Also, get extra help from our support team on [Slack](https://virgilsecurity.com/join-community).
