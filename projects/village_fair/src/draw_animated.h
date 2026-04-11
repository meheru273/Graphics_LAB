// This file is #included by main.cpp.
// It is part of the Village-fair project split for readability.
// All global variables and #includes are inherited from main.cpp.
//
// Each ride/object is now in its own component file:
//   draw_ferriswheel.h  — FerrisWheel + FerrisWheelSeat
//   draw_carousel.h     — Carousal (merry-go-round)
//   draw_props.h        — drawKite, drawJar, drawBezierRiver, drawRealisticTree, seat, glass
//   (PirateShip is now in draw_pond_ship.h together with the pond)
#pragma once

#include "src/draw_ferriswheel.h"
#include "src/draw_carousel.h"
#include "src/draw_props.h"