/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH) 
** version of the open source software sgems.  It is a derivative work by 
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original 
** version of the software (see notice below) and now sublicensed such that it 
** cannot be distributed or modified without the explicit and written permission 
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this 
** file/software.
**
** This file cannot be modified or distributed without the explicit and written 
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at 
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#include <grid/grid_model/cartesian_grid.h>

#include <GsTL/math/math_functions.h>

#include <numeric>

Named_interface* create_Cgrid( std::string& name) {
  return new Cartesian_grid(name, 1,1,1);
}


Cartesian_grid::Cartesian_grid(std::string name, int nx, int ny, int nz)
 : RGrid(name){
  set_dimensions( nx, ny, nz );
}

Cartesian_grid::Cartesian_grid(std::string name, int nx, int ny, int nz,
                            double xsize, double ysize, double zsize)
 : RGrid(name){
  set_dimensions( nx, ny, nz, xsize, ysize, zsize );
}

void Cartesian_grid::set_dimensions( int nx, int ny, int nz ) {
  // set up a new geometry and pass it to the rgrid
  Simple_RGrid_geometry* geometry = new Simple_RGrid_geometry();
  geometry->set_size(0, nx);
  geometry->set_size(1, ny);
  geometry->set_size(2, nz);

  set_geometry(geometry);

  geometry_ = dynamic_cast<Simple_RGrid_geometry*>( RGrid::geom_ );
}

void Cartesian_grid::set_dimensions( int nx, int ny, int nz,
				     double xsize, double ysize, double zsize ) {
  Cartesian_grid::set_dimensions( nx, ny, nz );
  GsTLCoordVector dims( xsize, ysize, zsize );
  geometry_->set_cell_dims( dims );
}


void Cartesian_grid::set_geometry( RGrid_geometry* geom ) {
  if( !dynamic_cast<Simple_RGrid_geometry*>( geom ) ) return;

  if( geom_ != geom ) {
    delete geom_;
    geom_ = geom->clone();
    connection_is_updated_ = false;
  }
  grid_cursor_ = new SGrid_cursor(nx(), ny(), nz(), 1);
  property_manager_.set_prop_size( geom->size() );
  region_manager_.set_region_size( geom->size() );


  geometry_ = dynamic_cast<Simple_RGrid_geometry*>( RGrid::geom_ );
}



GsTLInt Cartesian_grid::closest_node( const location_type& P ) const {
  location_type origin = geometry_->origin();
  location_type P0;
  P0.x() = P.x() - origin.x();
  P0.y() = P.y() - origin.y();
  P0.z() = P.z() - origin.z();
 
  GsTLCoordVector cell_sizes = geometry_->cell_dims();
  int spacing = grid_cursor_->multigrid_spacing();
 
  // Account for the multi-grid spacing
  cell_sizes.x() = cell_sizes.x() * spacing;
  cell_sizes.y() = cell_sizes.y() * spacing;
  cell_sizes.z() = cell_sizes.z() * spacing;

  GsTLInt i = std::max( GsTL::floor( P0.x()/cell_sizes.x() + 0.5 ), 0 );
  GsTLInt j = std::max( GsTL::floor( P0.y()/cell_sizes.y() + 0.5 ), 0 );
  GsTLInt k = std::max( GsTL::floor( P0.z()/cell_sizes.z() + 0.5 ), 0 );
  
  // The function will return a valid node even if P is outside the
  // grid's bounding box  
  if( i >= grid_cursor_->max_iter( SGrid_cursor::X ) )
    i = grid_cursor_->max_iter( SGrid_cursor::X ) - 1;
  if( j >= grid_cursor_->max_iter( SGrid_cursor::Y ) )  
    j = grid_cursor_->max_iter( SGrid_cursor::Y ) - 1;
  if( k >= grid_cursor_->max_iter( SGrid_cursor::Z ) )
    k = grid_cursor_->max_iter( SGrid_cursor::Z ) - 1;
  
  return grid_cursor_->node_id( i, j, k );
}


GsTL_cube Cartesian_grid::bounding_box() const {
  GsTLCoordVector sizes = geometry_->cell_dims();
  
  const GsTLCoord half = 0.5;
  GsTLCoordVector max_coord;
  max_coord.x() = geometry_->dim(0) * sizes.x();
  max_coord.y() = geometry_->dim(1) * sizes.y();
  max_coord.z() = geometry_->dim(2) * sizes.z();
  
  GsTLPoint O = geometry_->origin();

  GsTL_box::location_type lower( O + ( -half*sizes) );
  GsTL_box::location_type upper( O + max_coord + ( half*sizes ) );
  
  return GsTL_cube( lower, upper );
}
