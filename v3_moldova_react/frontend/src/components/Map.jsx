import React, { useMemo } from 'react';
import { MapContainer, TileLayer, Marker, Popup, Polyline, useMap } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';

// Fix default marker icon in webpack/CRA
delete L.Icon.Default.prototype._getIconUrl;
L.Icon.Default.mergeOptions({
  iconUrl: require('leaflet/dist/images/marker-icon.png'),
  iconRetinaUrl: require('leaflet/dist/images/marker-icon-2x.png'),
  shadowUrl: require('leaflet/dist/images/marker-shadow.png'),
});

const ROUTE_COLORS = ['#dc2626', '#059669', '#2563eb', '#7c3aed', '#ea580c', '#0891b2', '#4f46e5', '#be185d'];

function FitBounds({ cities, routes, selectedRouteIndex }) {
  const map = useMap();
  const bounds = useMemo(() => {
    const pts = [];
    cities.forEach((c) => pts.push([c.lat, c.lng]));
    if (selectedRouteIndex != null && routes[selectedRouteIndex]?.cities) {
      routes[selectedRouteIndex].cities.forEach((c) => pts.push([c.lat, c.lng]));
    }
    if (pts.length === 0) return null;
    return L.latLngBounds(pts);
  }, [cities, routes, selectedRouteIndex]);
  React.useEffect(() => {
    if (bounds) map.fitBounds(bounds, { padding: [20, 20], maxZoom: 10 });
  }, [map, bounds]);
  return null;
}

function Map({ cities, routes, selectedRouteIndex }) {
  const position = useMemo(() => {
    if (cities.length) {
      const lat = cities.reduce((s, c) => s + c.lat, 0) / cities.length;
      const lng = cities.reduce((s, c) => s + c.lng, 0) / cities.length;
      return [lat, lng];
    }
    return [47.2, 27.0];
  }, [cities]);

  return (
    <MapContainer
      center={position}
      zoom={8}
      className="h-full w-full"
      scrollWheelZoom
    >
      <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a>'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
      />
      <FitBounds cities={cities} routes={routes} selectedRouteIndex={selectedRouteIndex} />
      {cities.map((city) => (
        <Marker key={city.id} position={[city.lat, city.lng]}>
          <Popup>{city.name}</Popup>
        </Marker>
      ))}
      {routes.map((route, idx) => {
        const positions = (route.cities || []).map((c) => [c.lat, c.lng]);
        const isSelected = idx === selectedRouteIndex;
        const color = ROUTE_COLORS[idx % ROUTE_COLORS.length];
        return (
          <Polyline
            key={idx}
            positions={positions}
            pathOptions={{
              color,
              weight: isSelected ? 6 : 3,
              opacity: isSelected ? 1 : 0.6,
            }}
            eventHandlers={{
              click: () => {},
            }}
          />
        );
      })}
    </MapContainer>
  );
}

export default Map;
