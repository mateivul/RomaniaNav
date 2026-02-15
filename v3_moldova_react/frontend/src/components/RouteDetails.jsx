import React from 'react';

function segmentColor(roadType) {
  if (!roadType) return 'text-gray-600';
  if (roadType.startsWith('E')) return 'text-blue-700';
  if (roadType.startsWith('DN')) return 'text-green-700';
  if (roadType.startsWith('DJ')) return 'text-amber-700';
  return 'text-gray-700';
}

function RouteDetails({ route }) {
  const segments = route.segments || [];
  const totalDistance = route.totalDistance ?? segments.reduce((s, seg) => s + (seg.distance || 0), 0);
  const totalTime = route.totalTime ?? segments.reduce((s, seg) => {
    const dist = seg.distance || 0;
    const speed = seg.speedLimit || 80;
    return s + dist / speed;
  }, 0);

  return (
    <div className="p-4">
      <h3 className="font-semibold text-gray-800 border-b pb-2 mb-3">Detalii traseu</h3>
      <ul className="space-y-2">
        {segments.map((seg, i) => {
          const timeH = (seg.distance || 0) / (seg.speedLimit || 80);
          return (
            <li key={i} className="text-sm border-l-2 border-gray-200 pl-3 py-1">
              <span className="font-medium text-gray-700">{seg.from}</span>
              <span className="text-gray-400 mx-1">→</span>
              <span className="font-medium text-gray-700">{seg.to}</span>
              <div className="mt-0.5 flex flex-wrap gap-x-3 gap-y-0 text-gray-600">
                <span>{seg.distance} km</span>
                <span className={segmentColor(seg.roadType)}>{seg.roadType}</span>
                <span>{seg.speedLimit} km/h</span>
                <span>~{(timeH * 60).toFixed(0)} min</span>
              </div>
            </li>
          );
        })}
      </ul>
      <div className="mt-4 pt-3 border-t font-medium text-gray-800">
        Total: {totalDistance.toFixed(1)} km · ~{totalTime.toFixed(2)} h
      </div>
    </div>
  );
}

export default RouteDetails;
