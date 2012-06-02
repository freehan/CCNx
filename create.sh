#!/bin/bash

# $1: Directory name
# $2: Slice name

mkdir $1
ccnsyncslice create /$2 /$2/
