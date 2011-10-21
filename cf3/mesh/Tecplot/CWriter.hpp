// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef cf3_mesh_Tecplot_CWriter_hpp
#define cf3_mesh_Tecplot_CWriter_hpp

////////////////////////////////////////////////////////////////////////////////

#include "mesh/MeshWriter.hpp"
#include "mesh/GeoShape.hpp"

#include "mesh/Tecplot/LibTecplot.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace cf3 {
namespace mesh {
  class ElementType;
namespace Tecplot {

//////////////////////////////////////////////////////////////////////////////

/// This class defines Tecplot mesh format writer
/// @author Willem Deconinck
class Tecplot_API CWriter : public MeshWriter
{
public: // typedefs

    typedef boost::shared_ptr<CWriter> Ptr;
    typedef boost::shared_ptr<CWriter const> ConstPtr;

public: // functions

  /// constructor
  CWriter( const std::string& name );

  /// Gets the Class name
  static std::string type_name() { return "CWriter"; }

  virtual void write_from_to(const Mesh& mesh, const common::URI& path);

  virtual std::string get_format() { return "Tecplot"; }

  virtual std::vector<std::string> get_extensions();

private: // functions

  void write_file(std::fstream& file);

  std::string zone_type(const ElementType& etype) const;

private: // data


}; // end CWriter


////////////////////////////////////////////////////////////////////////////////

} // Tecplot
} // mesh
} // cf3

////////////////////////////////////////////////////////////////////////////////

#endif // cf3_mesh_Tecplot_CWriter_hpp
