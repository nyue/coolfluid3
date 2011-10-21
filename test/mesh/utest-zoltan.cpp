// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for Zoltan load balancing library"

#include <boost/test/unit_test.hpp>

#include "common/Log.hpp"
#include "common/Core.hpp"
#include "common/Environment.hpp"
#include "common/Root.hpp"
#include "common/Foreach.hpp"
#include "common/OSystem.hpp"
#include "common/OSystemLayer.hpp"

#include "common/PE/Comm.hpp"
#include "common/PE/debug.hpp"

#include "mesh/Mesh.hpp"
#include "mesh/Geometry.hpp"
#include "mesh/CRegion.hpp"
#include "mesh/MeshReader.hpp"
#include "mesh/MeshWriter.hpp"
#include "mesh/MeshGenerator.hpp"
#include "mesh/MeshPartitioner.hpp"
#include "mesh/MeshTransformer.hpp"

using namespace boost;
using namespace cf3;
using namespace cf3::mesh;
using namespace cf3::common;
using namespace cf3::common::PE;

////////////////////////////////////////////////////////////////////////////////

struct ZoltanTests_Fixture
{
  /// common setup for each test case
  ZoltanTests_Fixture()
  {
    // uncomment if you want to use arguments to the test executable
    m_argc = boost::unit_test::framework::master_test_suite().argc;
    m_argv = boost::unit_test::framework::master_test_suite().argv;

  }

  /// common tear-down for each test case
  ~ZoltanTests_Fixture()
  {

  }

  /// possibly common functions used on the tests below

  int m_argc;
  char** m_argv;
};


////////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE( ZoltanTests_TestSuite, ZoltanTests_Fixture )

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( init_mpi )
{
  Core::instance().initiate(m_argc,m_argv);
  PE::Comm::instance().init(m_argc,m_argv);

}

////////////////////////////////////////////////////////////////////////////////
/*
BOOST_AUTO_TEST_CASE( MeshPartitioner_test_quadtriag )
{
  Core::instance().environment().configure_option("log_level",(Uint)DEBUG);
  CFinfo << "MeshPartitioner_test" << CFendl;
  MeshReader::Ptr meshreader = build_component_abstract_type<MeshReader>("CF.Mesh.Neu.CReader","meshreader");
  meshreader->configure_option("read_boundaries",false);

  // the file to read from
  URI fp_in ("quadtriag.neu");

  // the mesh to store in
  Mesh::Ptr mesh_ptr = meshreader->create_mesh_from(fp_in);
  Mesh& mesh = *mesh_ptr;

  MeshTransformer::Ptr glb_numbering = build_component_abstract_type<MeshTransformer>("CF.Mesh.Actions.CGlobalNumbering","glb_numbering");
  glb_numbering->transform(mesh_ptr);
  MeshTransformer::Ptr glb_connectivity = build_component_abstract_type<MeshTransformer>("CF.Mesh.Actions.CGlobalConnectivity","glb_connectivity");
  glb_connectivity->transform(mesh_ptr);

  MeshWriter::Ptr meshwriter = build_component_abstract_type<MeshWriter>("CF.Mesh.Gmsh.CWriter","meshwriter");
  URI fp_out_1 ("quadtriag.msh");
  meshwriter->write_from_to(*mesh_ptr,fp_out_1);

  MeshPartitioner::Ptr partitioner_ptr = build_component_abstract_type<MeshTransformer>("CF.Mesh.Zoltan.CPartitioner","partitioner")->as_ptr<MeshPartitioner>();

  MeshPartitioner& p = *partitioner_ptr;
  BOOST_CHECK_EQUAL(p.name(),"partitioner");

  Core::instance().initiate(m_argc,m_argv);

  //p.configure_option("nb_parts", (Uint) 4);
  p.configure_option("graph_package", std::string("PHG"));
  p.configure_option("debug_level", 2u);
  BOOST_CHECK(true);
  p.initialize(mesh);

  BOOST_CHECK_EQUAL(p.proc_of_obj(0), 0u);
  BOOST_CHECK_EQUAL(p.proc_of_obj(7), 0u);
  BOOST_CHECK_EQUAL(p.proc_of_obj(8), 0u);
  BOOST_CHECK_EQUAL(p.proc_of_obj(15), 0u);
  BOOST_CHECK_EQUAL(p.proc_of_obj(16), 1u);
  BOOST_CHECK_EQUAL(p.proc_of_obj(23), 1u);
  BOOST_CHECK_EQUAL(p.proc_of_obj(24), 1u);
  BOOST_CHECK_EQUAL(p.proc_of_obj(31), 1u);

  BOOST_CHECK_EQUAL(p.is_node(0), true);
  BOOST_CHECK_EQUAL(p.is_node(7), true);
  BOOST_CHECK_EQUAL(p.is_node(8), false);
  BOOST_CHECK_EQUAL(p.is_node(15), false);
  BOOST_CHECK_EQUAL(p.is_node(16), true);
  BOOST_CHECK_EQUAL(p.is_node(23), true);
  BOOST_CHECK_EQUAL(p.is_node(24), false);
  BOOST_CHECK_EQUAL(p.is_node(31), false);

  Uint comp_idx;
  Component::Ptr comp;
  Uint idx;
  bool found;
  if ( PE::Comm::instance().rank() == 0)
  {
    boost::tie(comp,idx) = p.to_local(0);
    boost::tie(comp_idx,idx,found) = p.to_local_indices_from_glb_obj(0);
    BOOST_CHECK( is_not_null(comp->as_ptr<Geometry>()) );
    BOOST_CHECK_EQUAL(comp_idx, 0);
    BOOST_CHECK_EQUAL(idx, 0);
    BOOST_CHECK_EQUAL(found, true);

    boost::tie(comp_idx,idx,found) = p.to_local_indices_from_glb_obj(7);
    BOOST_CHECK_EQUAL(comp_idx, 0);
    BOOST_CHECK_EQUAL(idx, 7);
    BOOST_CHECK_EQUAL(found, true);

  }

  BOOST_CHECK(true);
  p.partition_graph();
  BOOST_CHECK(true);
  p.show_changes();
  BOOST_CHECK(true);
  p.migrate();
  BOOST_CHECK(true);

  MeshTransformer::Ptr glb_node_numbering = build_component_abstract_type<MeshTransformer>("CF.Mesh.Actions.CGlobalNumberingNodes","glb_node_numbering");
  glb_node_numbering->configure_option("debug",true);
  glb_node_numbering->transform(mesh);


  PEProcessSortedExecute(-1,
      std::cout << "rank  = "  << Comm::instance().rank() << std::endl;
      std::cout << "nodes = " << mesh.geometry().glb_idx() << std::endl;
      std::cout << "ranks = " << mesh.geometry().rank() << std::endl;
      boost_foreach(const CEntities& entities, mesh.topology().elements_range())
      {
        //std::cout << "elems = " << entities.glb_idx() << std::endl;
      }

  )

  URI fp_out_2 ("quadtriag_repartitioned.msh");
  meshwriter->write_from_to(*mesh_ptr,fp_out_2);
}
*/
BOOST_AUTO_TEST_CASE( MeshPartitioner_test_quadtriag )
{
  Core::instance().environment().configure_option("log_level",(Uint)DEBUG);
  MeshGenerator::Ptr meshgenerator = build_component_abstract_type<MeshGenerator>("CF.Mesh.CSimpleMeshGenerator","1Dgenerator");

  meshgenerator->configure_option("mesh",URI("//Root/rect"));
  std::vector<Uint> nb_cells(2);  nb_cells[0] = 3;   nb_cells[1] = 2;
  std::vector<Real> lengths(2);   lengths[0]  = nb_cells[0];  lengths[1]  = nb_cells[1];
  meshgenerator->configure_option("nb_cells",nb_cells);
  meshgenerator->configure_option("lengths",lengths);
  meshgenerator->configure_option("bdry",false);
  Mesh& mesh = meshgenerator->generate();


  MeshTransformer::Ptr glb_numbering = build_component_abstract_type<MeshTransformer>("CF.Mesh.Actions.CGlobalNumbering","glb_numbering");
  glb_numbering->transform(mesh);
  MeshTransformer::Ptr glb_connectivity = build_component_abstract_type<MeshTransformer>("CF.Mesh.Actions.CGlobalConnectivity","glb_connectivity");
  glb_connectivity->transform(mesh);

  MeshWriter::Ptr meshwriter = build_component_abstract_type<MeshWriter>("CF.Mesh.Gmsh.CWriter","meshwriter");
  meshwriter->write_from_to(mesh,"rect.msh");

  MeshPartitioner::Ptr partitioner_ptr = build_component_abstract_type<MeshTransformer>("CF.Mesh.Zoltan.CPartitioner","partitioner")->as_ptr<MeshPartitioner>();

  MeshPartitioner& p = *partitioner_ptr;
  BOOST_CHECK_EQUAL(p.name(),"partitioner");

  Core::instance().initiate(m_argc,m_argv);

  //p.configure_option("nb_parts", (Uint) 4);
  p.configure_option("graph_package", std::string("PHG"));
  p.configure_option("debug_level", 2u);
  BOOST_CHECK(true);
  p.initialize(mesh);
  BOOST_CHECK(true);
  p.partition_graph();
  BOOST_CHECK(true);
  p.show_changes();
  BOOST_CHECK(true);
  p.migrate();
  BOOST_CHECK(true);

  MeshTransformer::Ptr glb_node_numbering = build_component_abstract_type<MeshTransformer>("CF.Mesh.Actions.CGlobalNumberingNodes","glb_node_numbering");
  glb_node_numbering->configure_option("debug",true);
  glb_node_numbering->transform(mesh);


  PEProcessSortedExecute(-1,
      std::cout << PERank << "nodes = " << mesh.geometry().coordinates() << std::endl;
      std::cout << PERank << "ranks = " << mesh.geometry().rank() << std::endl;
      boost_foreach(const CEntities& entities, mesh.topology().elements_range())
      {
        //std::cout << "elems = " << entities.glb_idx() << std::endl;
      }

  )


  MeshWriter::Ptr tecwriter = build_component_abstract_type<MeshWriter>("CF.Mesh.Tecplot.CWriter","meshwriter");
  tecwriter->write_from_to(mesh,"rect_repartitioned.plt");
  meshwriter->write_from_to(mesh,"rect_repartitioned.msh");
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( finalize_mpi )
{
  PE::Comm::instance().finalize();

  Core::instance().terminate();
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////

