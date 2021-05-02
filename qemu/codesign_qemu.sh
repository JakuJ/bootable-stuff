#!/bin/bash

codesign -s - --entitlements app.entitlements --force "$(command -v qemu-system-x86_64)"
