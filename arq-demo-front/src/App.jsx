import { useState, useEffect } from 'react'
import { Card } from './Card'

const ws = new WebSocket('ws://192.168.0.110:3000')

function App() {
  const [actualMessage, setActualMessage] = useState([])

  useEffect(() => {
    const handleReceivedMessage = (event) => {
      const message = JSON.parse(event.data)
      setActualMessage([message])
    }
    ws.addEventListener('message', handleReceivedMessage)

    return () => {
      ws.removeEventListener('message', handleReceivedMessage)
    }
  }, [])

  return (
    <>
      <div className="w-[500px] border-2 border-sky-300 rounded-lg p-6">
        <h1 className="text-6xl text-center">Heart Rate App</h1>
        <div className="mt-6">
          { actualMessage.map(({ ir, bpm, avg_bpm, finger }, index) => (
            <Card
              key={ index }
              avg_bpm={ avg_bpm }
              ir={ ir } bpm={ bpm }
              finger={ finger }
            />
          ))}
        </div>
      </div>
    </>
  )
}

export default App
