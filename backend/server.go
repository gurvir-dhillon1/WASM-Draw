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
	"strings"
)

// Enums for the different drawing types
const (
    LINE = iota
    CIRCLE
    RECT
	ERASE
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

type WebSocketMessage struct {
	Type	string			`json:"type"`
	Payload	json.RawMessage	`json:"payload"`
}

type Room struct {
	clients	map[*websocket.Conn]bool
	mu		sync.Mutex
}

var mu sync.Mutex
var upgrader = websocket.Upgrader{
    CheckOrigin: func(r *http.Request) bool {
        return true
    },
}

// map of current rooms
var rooms = make(map[string]*Room)
var globalMu sync.Mutex

var allowedOrigins []string

func generateRoomCode(length int) string {
	const charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz"
	rand.New(rand.NewSource(time.Now().UnixNano()))

	code := make([]byte, length)

	for i := range code {
		code[i] = charset[rand.Intn(len(charset))]
	}

	return string(code)
}

func createRoom(w http.ResponseWriter, r *http.Request) {
	origin := r.Header.Get("Origin")

	for _, allowed := range allowedOrigins {
		if origin == allowed || allowed == "*" {
			w.Header().Set("Access-Control-Allow-Origin", origin)
			break
		}
	}

	w.Header().Set("Content-Type", "application/json")
	w.Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
	w.Header().Set("Access-Control-Allow-Headers", "Content-Type")

	if r.Method == http.MethodOptions {
		// preflight request; return 200 with headers
		w.WriteHeader(http.StatusOK)
		return
	}
	roomCode := generateRoomCode(6)

	// add room to global rooms map without causing race conditions
	globalMu.Lock()
	rooms[roomCode] = &Room{
		clients: make(map[*websocket.Conn]bool),
	}
	globalMu.Unlock()

	// send { "room" : roomCode } back to frontend/client so the frontend knows
	// what room was just created
	json.NewEncoder(w).Encode(map[string]string{
		"room": roomCode,
	})
}

func handleWebSocket(w http.ResponseWriter, r *http.Request) {
	roomCode := r.URL.Query().Get("room")
	if roomCode == "" {
		http.Error(w, "Missing room parameter", http.StatusBadRequest)
		return
	}

	globalMu.Lock()
	room, exists := rooms[roomCode]
	globalMu.Unlock()

	if !exists {
		http.Error(w, "Invalid room code", http.StatusNotFound)
		return
	}

    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
        log.Println("Upgrade: ", err)
        return
    }
    defer conn.Close()

	room.mu.Lock()
	room.clients[conn] = true
	room.mu.Unlock()

	log.Printf("Client joined room %s", roomCode)

    for {
        messageType, payload, err := conn.ReadMessage()
        if err != nil {
            log.Println("ReadMessage: ", err)
			room.mu.Lock()
			delete(room.clients, conn)
			isEmpty := len(room.clients) == 0
			room.mu.Unlock()

			if isEmpty {
				globalMu.Lock()
				delete(rooms, roomCode)
				globalMu.Unlock()
				log.Printf("Room %s deleted (empty)", roomCode)
			}
			log.Printf("Client disconnected from room %s", roomCode)
            return
        }

        var msg WebSocketMessage
        err = json.Unmarshal(payload, &msg)
        if err != nil {
            log.Println("Unmarshal: ", err)
            return
        }
		switch msg.Type {
			case "draw":
				var drawCommands []DrawCommand
				err = json.Unmarshal(msg.Payload, &drawCommands)
				if err != nil {
					log.Println("Unmarshal DrawCommands:", err)
					continue
				}
				broadcastData, err := json.Marshal(WebSocketMessage{
					Type: "draw",
					Payload: msg.Payload,
				})
				if err != nil {
					log.Println("Marshal: ", err)
					return
				}

				room.mu.Lock()
				for client := range room.clients {
					if client != conn {
						err = client.WriteMessage(messageType, broadcastData)
						if err != nil {
							log.Println("WriteMessage: ", err)
							client.Close()
							delete(room.clients, client)
						}
					}
				}
				room.mu.Unlock()
			case "clear-canvas":
				broadcastData, err := json.Marshal(WebSocketMessage{ Type: "clear-canvas" })
				if err != nil {
					log.Println("Marshal:", err)
					continue
				}
				room.mu.Lock()
				for client := range room.clients {
					err = client.WriteMessage(messageType, broadcastData)
					if err != nil {
						log.Println("WriteMessage: ", err)
					}
				}
				room.mu.Unlock()
			default:
				log.Println("Unknown message type:", msg.Type)
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
	corsOrigin := os.Getenv("ALLOWED_ORIGINS")
	if corsOrigin == "" {
		allowedOrigins = []string{
			"http://localhost:8000",
			"https://wasm-draw.art",
		}
	} else {
		allowedOrigins = strings.Split(corsOrigin, ",")
		for i, origin := range allowedOrigins {
			allowedOrigins[i] = strings.TrimSpace(origin)
		}
	}
    if (port == "" || host == "") {
        log.Fatal("host or port env variables not defined")
    }

    address := host + ":" + port
    http.HandleFunc("/ws", handleWebSocket)
	http.HandleFunc("/create", createRoom)

    log.Printf("server started at %v", address)

    err = http.ListenAndServe(address, nil)
    if err != nil {
        log.Fatal("ListenAndServe: ", err)
    }
}
