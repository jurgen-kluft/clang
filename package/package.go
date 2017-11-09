package xlang

import (
	"github.com/jurgen-kluft/xcode/denv"
	"github.com/jurgen-kluft/xbase/package"
	"github.com/jurgen-kluft/xentry/package"
	"github.com/jurgen-kluft/xunittest/package"
)

// GetPackage returns the package object of 'xlang'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := xunittest.GetPackage()
	entrypkg := xentry.GetPackage()
	basepkg := xbase.GetPackage()

	// The main (xlang) package
	mainpkg := denv.NewPackage("xlang")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(entrypkg)
	mainpkg.AddPackage(basepkg)

	// 'xlang' library
	mainlib := denv.SetupDefaultCppLibProject("xlang", "github.com\\jurgen-kluft\\xlang")
	mainlib.Dependencies = append(mainlib.Dependencies, basepkg.GetMainLib())

	// 'xlang' unittest project
	maintest := denv.SetupDefaultCppTestProject("xlang_test", "github.com\\jurgen-kluft\\xlang")
	maintest.Dependencies = append(maintest.Dependencies, unittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, entrypkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)

	return mainpkg
}
