package main

import (
	"github.com/jurgen-kluft/xcode"
	"github.com/jurgen-kluft/xlang/package"
)

func main() {
	xcode.Init()
	xcode.Generate(xlang.GetPackage())
}
