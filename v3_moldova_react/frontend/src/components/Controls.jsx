import React, { useState } from 'react';
import { Navigation } from 'lucide-react';

function Controls({ cities, onCalculate, loading }) {
  const [source, setSource] = useState('');
  const [destination, setDestination] = useState('');

  const handleSubmit = (e) => {
    e.preventDefault();
    const s = parseInt(source, 10);
    const d = parseInt(destination, 10);
    if (Number.isNaN(s) || Number.isNaN(d) || s === d) return;
    onCalculate(s, d);
  };

  return (
    <div className="p-4 border-b">
      <h2 className="font-semibold text-gray-800 mb-3">Trasee</h2>
      <form onSubmit={handleSubmit} className="space-y-3">
        <div>
          <label className="block text-sm text-gray-600 mb-1">Plecare</label>
          <select
            className="w-full border rounded px-3 py-2 bg-white"
            value={source}
            onChange={(e) => setSource(e.target.value)}
          >
            <option value="">Alege oraș</option>
            {cities.map((c) => (
              <option key={c.id} value={c.id}>{c.name}</option>
            ))}
          </select>
        </div>
        <div>
          <label className="block text-sm text-gray-600 mb-1">Destinație</label>
          <select
            className="w-full border rounded px-3 py-2 bg-white"
            value={destination}
            onChange={(e) => setDestination(e.target.value)}
          >
            <option value="">Alege oraș</option>
            {cities.map((c) => (
              <option key={c.id} value={c.id}>{c.name}</option>
            ))}
          </select>
        </div>
        <button
          type="submit"
          disabled={loading || !source || !destination || source === destination}
          className="w-full flex items-center justify-center gap-2 bg-emerald-600 text-white rounded py-2 font-medium disabled:opacity-50 disabled:cursor-not-allowed hover:bg-emerald-700"
        >
          <Navigation size={18} />
          {loading ? 'Se calculează...' : 'Calculează Trasee'}
        </button>
      </form>
    </div>
  );
}

export default Controls;
