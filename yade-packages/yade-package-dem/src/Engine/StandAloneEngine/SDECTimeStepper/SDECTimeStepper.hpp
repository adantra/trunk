/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SDECTIMESTEPPER_HPP__
#define __SDECTIMESTEPPER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/TimeStepper.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Interaction;
class BodyContainer;
class MacroMicroElasticRelationships;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SDECTimeStepper : public TimeStepper
{

	public  : int sdecGroupMask;
	private : shared_ptr<MacroMicroElasticRelationships> sdecContactModel;

	public : SDECTimeStepper();
	public : virtual ~SDECTimeStepper();
	
	public : virtual void computeTimeStep(Body* body);
	
	private : void findTimeStepFromBody(const shared_ptr<Body>&);
	private : void findTimeStepFromInteraction(const shared_ptr<Interaction>& , shared_ptr<BodyContainer>&);
	private : Real newDt;
	private : bool computedSomething;

	protected: virtual void registerAttributes();
	REGISTER_CLASS_NAME(SDECTimeStepper);
	REGISTER_BASE_CLASS_NAME(TimeStepper);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(SDECTimeStepper,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SDECTIMESTEPPER_HPP__

