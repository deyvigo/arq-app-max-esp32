import express, { Response } from 'express'
import cors from 'cors'
import { Server as SocketServer } from 'socket.io'
import http from 'node:http'

const app = express()

const PORT = 3000

app.use(cors({
  origin: '*', // Permitir solo este origen
  methods: ['GET', 'POST', 'OPTIONS'], // Métodos permitidos
  allowedHeaders: ['Content-Type'], // Encabezados permitidos
}))

const server = http.createServer(app)

// Configurar CORS para Socket.io
const io = new SocketServer(server, {
  cors: {
    origin: '*', // Permitir solo este origen
    methods: ['GET', 'POST'], // Métodos permitidos
    allowedHeaders: ['Content-Type'], // Encabezados permitidos
  }
})

io.on('connection', (socket) => {
  console.log(socket.id)
  socket.on('message', (body) => {
    socket.broadcast.emit('message', body)
  })
})

app.get('/helloworld', (_, res: Response) => {
  res.send({
    "saludo": "Hello World"
  })
})

server.listen(PORT, '0.0.0.0', () => {
  console.log(`Server on http://0.0.0.0:${PORT}`)
})
