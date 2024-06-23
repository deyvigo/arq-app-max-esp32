import cors from 'cors'
import express, { Response } from 'express'
import WebSocket from 'ws'
import http from 'node:http'

const PORT = 3000

const app = express()

app.use(cors())

const server = http.createServer(app)

const wss = new WebSocket.Server({ server })

interface Message {
  ir: string,
  bpm: string,
  avg_bpm: string,
  finger: string
}

wss.on('connection', (ws: WebSocket) => {
  console.log('Cliente conectado')

  ws.on('message', (data: string) => {
    // console.log(`Mensaje recibido: ${data}`)
    // console.log(typeof data)
    const message: Message = JSON.parse(data)

    // Enviar el mensaje de vuelta al cliente
    wss.clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(JSON.stringify(message))
      }
    })
  })

  // Manejar cierre de la conexión
  ws.on('close', () => {
    console.log('Cliente desconectado')
  })
})

app.get('/helloworld', (_, res: Response) => {
  res.send({ "message": "Hello World" })
})

server.listen(PORT, '0.0.0.0', () => {
  console.log(`Servidor escuchando en el puerto ${PORT}`)
});
