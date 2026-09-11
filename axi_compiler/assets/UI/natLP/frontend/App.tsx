import { Routes, Route } from 'react-router-dom'
import NatLPConsole from './pages/NatLPConsole'

export default function App() {
  return (
    <Routes>
      <Route path="/" element={<NatLPConsole />} />
    </Routes>
  )
}
