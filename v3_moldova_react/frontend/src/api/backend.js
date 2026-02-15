import axios from 'axios';

// Use relative URL so dev server proxies to backend (see "proxy" in package.json)
const API_BASE_URL = '/api';

export const getCities = () => axios.get(`${API_BASE_URL}/cities`);
export const getHealth = () => axios.get(`${API_BASE_URL}/health`);
export const calculateRoutes = (source, destination) =>
  axios.post(`${API_BASE_URL}/calculate`, { source, destination });
