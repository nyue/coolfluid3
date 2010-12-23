// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_RDM_LibRDM_hpp
#define CF_RDM_LibRDM_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/CLibrary.hpp"

////////////////////////////////////////////////////////////////////////////////

/// Define the macro RDM_API
/// @note build system defines COOLFLUID_BLOCKMESH_READER_EXPORTS when compiling
/// RDM files
#ifdef COOLFLUID_BLOCKMESH_READER_EXPORTS
#   define RDM_API      CF_EXPORT_API
#   define RDM_TEMPLATE
#else
#   define RDM_API      CF_IMPORT_API
#   define RDM_TEMPLATE CF_TEMPLATE_EXTERN
#endif

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace RDM {

////////////////////////////////////////////////////////////////////////////////

/// Class defines the RDM finite elment method library
/// @author Bart Janssens
class RDM_API LibRDM :
    public Common::CLibrary
{
public:

  typedef boost::shared_ptr<LibRDM> Ptr;
  typedef boost::shared_ptr<LibRDM const> ConstPtr;

  /// Constructor
  LibRDM ( const std::string& name) : Common::CLibrary(name) {   }

public: // functions

  /// @return string of the library namespace
  static std::string library_namespace() { return "CF.RDM"; }

  /// Static function that returns the module name.
  /// Must be implemented for CLibrary registration
  /// @return name of the library
  static std::string library_name() { return "RDM"; }

  /// Static function that returns the description of the module.
  /// Must be implemented for CLibrary registration
  /// @return description of the library

  static std::string library_description()
  {
    return "This library implements a Residual Distribution Solver.";
  }

  /// Gets the Class name
  static std::string type_name() { return "LibRDM"; }

  /// initiate library
  virtual void initiate();

  /// terminate library
  virtual void terminate();

}; // end LibRDM

////////////////////////////////////////////////////////////////////////////////

} // RDM
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_RDM_LibRDM_hpp
