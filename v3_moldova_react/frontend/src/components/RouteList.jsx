import React from 'react';

function RouteList({ routes, selectedIndex, onSelectRoute }) {
  if (!routes.length) {
    return (
      <div className="p-4 text-gray-500 text-sm">
        Selectează plecare și destinație, apoi apasă Calculează Trasee.
      </div>
    );
  }

  return (
    <div className="flex-1 overflow-y-auto">
      <h3 className="px-4 py-2 font-semibold text-gray-800 border-b bg-gray-50">
        Trasee găsite ({routes.length})
      </h3>
      <ul className="divide-y">
        {routes.map((route, idx) => {
          const isSelected = idx === selectedIndex;
          const isFirst = idx === 0;
          return (
            <li
              key={idx}
              role="button"
              tabIndex={0}
              onClick={() => onSelectRoute(idx)}
              onKeyDown={(e) => e.key === 'Enter' && onSelectRoute(idx)}
              className={`px-4 py-3 cursor-pointer transition ${isSelected ? 'bg-emerald-50 border-l-4 border-emerald-600' : 'hover:bg-gray-50'}`}
            >
              <div className="flex items-center justify-between gap-2">
                <span className="font-medium text-gray-800">Traseu {idx + 1}</span>
                {isFirst && (
                  <span className="text-xs bg-emerald-600 text-white px-2 py-0.5 rounded">OPTIM</span>
                )}
              </div>
              <div className="text-sm text-gray-600 mt-0.5">
                {route.totalDistance?.toFixed(1)} km · ~{route.totalTime?.toFixed(2)} h
              </div>
            </li>
          );
        })}
      </ul>
    </div>
  );
}

export default RouteList;
