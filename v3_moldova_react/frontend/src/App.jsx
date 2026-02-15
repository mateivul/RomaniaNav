import React, { useState, useEffect } from 'react';
import { getCities, getHealth, calculateRoutes } from './api/backend';
import Map from './components/Map';
import Controls from './components/Controls';
import RouteList from './components/RouteList';
import RouteDetails from './components/RouteDetails';

function App() {
  const [cities, setCities] = useState([]);
  const [routes, setRoutes] = useState([]);
  const [selectedRouteIndex, setSelectedRouteIndex] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [backendOnline, setBackendOnline] = useState(false);

  useEffect(() => {
    getHealth()
      .then(() => setBackendOnline(true))
      .catch(() => setBackendOnline(false));
    getCities()
      .then((res) => setCities(res.data))
      .catch(() => setError('Nu s-au putut încărca orașele.'));
  }, []);

  const handleCalculate = (source, destination) => {
    setError(null);
    setRoutes([]);
    setSelectedRouteIndex(null);
    setLoading(true);
    calculateRoutes(source, destination)
      .then((res) => {
        setRoutes(res.data.routes || []);
        if (res.data.routes?.length) setSelectedRouteIndex(0);
        setLoading(false);
      })
      .catch((err) => {
        setError(err.response?.data?.error || 'Eroare la calculul traseelor.');
        setLoading(false);
      });
  };

  const selectedRoute = selectedRouteIndex != null && routes[selectedRouteIndex] ? routes[selectedRouteIndex] : null;

  return (
    <div className="h-screen flex flex-col bg-gray-100">
      <header className="bg-emerald-700 text-white px-4 py-2 flex items-center justify-between shadow">
        <h1 className="text-xl font-bold">Moldova Navigator</h1>
        <span
          className={`text-sm px-2 py-0.5 rounded ${backendOnline ? 'bg-green-500' : 'bg-red-500'}`}
        >
          {backendOnline ? 'Online' : 'Offline'}
        </span>
      </header>

      {error && (
        <div className="bg-red-100 border border-red-400 text-red-700 px-4 py-2 text-sm">
          {error}
        </div>
      )}

      <div className="flex-1 flex min-h-0">
        <aside className="w-80 flex flex-col bg-white border-r shadow-sm overflow-hidden">
          <Controls cities={cities} onCalculate={handleCalculate} loading={loading} />
          <RouteList
            routes={routes}
            selectedIndex={selectedRouteIndex}
            onSelectRoute={setSelectedRouteIndex}
          />
        </aside>
        <main className="flex-1 flex min-h-0">
          <div className="flex-1 min-w-0">
            <Map cities={cities} routes={routes} selectedRouteIndex={selectedRouteIndex} />
          </div>
          {selectedRoute && (
            <aside className="w-72 bg-white border-l shadow-sm overflow-y-auto">
              <RouteDetails route={selectedRoute} />
            </aside>
          )}
        </main>
      </div>
    </div>
  );
}

export default App;
