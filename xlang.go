package main

import (
	"github.com/jurgen-kluft/xlang/package"
	"github.com/jurgen-kluft/xcode"
)

func main() {
	xcode.Generate(xlang.GetPackage())
}
