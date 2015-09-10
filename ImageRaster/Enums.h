#pragma once

namespace States {
	enum AppState {
		Select, Marker, Measure, Calibration, Scale, Legend
	};

	enum MarkerType {
		Arrow, Box, Ellipse
	};	

	enum RulerState {
		Line, Rect, Circle, Circle2, Polygon, Path
	};
};