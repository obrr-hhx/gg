/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <vector>
#include <string>

#include "gg-model-base.hh"
#include "thunk.hh"

static const std::string GCC_COMPILER = ".gg/exe/bin/x86_64-linux-musl-gcc";
static const std::string AS = ".gg/exe/bin/as";
static const std::string CC1 = ".gg/exe/bin/cc1";

class GGModelCompile : public GGModelBase
{
protected:
  std::vector<gg::thunk::InFile> get_infiles()
  {
    return { srcfile, GCC_COMPILER, CC1 };
  }

public:
  GGModelCompile( int argc, char **argv ) : GGModelBase( argc, argv ) {}
  GGModelCompile( const std::vector<std::string> & args ) : GGModelBase( args ) {}

  ~GGModelCompile() {}
};

class GGModelAssemble : public GGModelBase
{
protected:
  std::vector<gg::thunk::InFile> get_infiles()
  {
    return { srcfile, GCC_COMPILER, AS };
  }

public:
  GGModelAssemble( int argc, char **argv ) : GGModelBase( argc, argv ) {}
  GGModelAssemble( const std::vector<std::string> & args ) : GGModelBase( args ) {}

  ~GGModelAssemble() {}
};