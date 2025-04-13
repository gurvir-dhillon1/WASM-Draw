package main

import (
    "github.com/gorilla/websocket"
    "encoding/json"
    "net/http"
    "log"
    "os"
    "sync"
    "github.com/joho/godotenv"
	"math/rand"
	"time"
)

// Enums for the different drawing types
const (
    FREE = iota
    LINE
    CIRCLE
    RECT
)

// this struct helps us Unmarshal and Marshal the incoming JSON bytes
// from the frontend
type DrawCommand struct {
    StartX int      `json:"startX"`
    StartY int      `json:"startY"`
    EndX   int      `json:"endX"`
    EndY   int      `json:"endY"`
    Type   int      `json:"type"`
}

var mu sync.Mutex
var upgrader = websocket.Upgrader{
    CheckOrigin: func(r *http.Request) bool {
        return true
    },
}

// map of current rooms
var rooms = make(map[string]map[*websocket.Conn]bool)
var roomMu sync.Mutex

func generateRoomCode(length int) string {
	const charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz"
	rand.Seed(time.Now().UnixNano())

	code := make([]byte, length)

	for i := range code {
		code[i] = charset[rand.Intn(len(charset))]
	}

	return string(code)
}

func createRoom(w http.ResponseWriter, r *http.Request) {
	roomCode := generateRoomCode(6)

	// add room to global rooms map without causing race conditions
	roomMu.Lock()
	rooms[roomCode] = make(map[*websocket.Conn]bool)
	roomMu.Unlock()

	// send { "room" : roomCode } back to frontend/client so the frontend knows
	// what room was just created
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]string{
		"room": roomCode,
	})
}

func serializePoints(w http.ResponseWriter, r *http.Request) {
    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
        log.Println("Upgrade: connection failed")
        return
    }

    clients[conn] = true
    defer conn.Close()
    log.Println("client connected")

    for {
        messageType, payload, err := conn.ReadMessage()
        if err != nil {
            log.Println("ReadMessage: err")
            return
        }

        if messageType == websocket.CloseMessage {
            log.Println("client disconnected")
            mu.Lock()
            delete(clients, conn)
            mu.Unlock()
            return
        }
        
        // serialize draw commands
        var drawCommands []DrawCommand
        err = json.Unmarshal([]byte(payload), &drawCommands)
        if err != nil {
            log.Println("Unmarshal: ", err)
            return
        }
        broadcastData, err := json.Marshal(drawCommands)
        if err != nil {
            log.Println("Marshal: ", err)
            return
        }

        for client := range clients {
            if client != conn {
                err = client.WriteMessage(messageType, broadcastData)
                if err != nil {
                    log.Println("WriteMessage: ", err)
                    client.Close()
                    mu.Lock()
                    delete(clients, client)
                    mu.Unlock()
                }
            }
        }
    }
}

func main() {
    log.SetFlags(0)
    err := godotenv.Load()
    if err != nil {
        log.Println("failed to load env files", err)
    }
    port := os.Getenv("PORT")
    host := os.Getenv("HOST")
    if (port == "" || host == "") {
        log.Fatal("host or port env variables not defined")
    }

    address := host + ":" + port
    http.HandleFunc("/ws", serializePoints)
	http.HandleFunc("/create", createRoom)

    log.Printf("server started at %v", address)

    err = http.ListenAndServe(address, nil)
    if err != nil {
        log.Fatal("ListenAndServe: ", err)
    }
}
