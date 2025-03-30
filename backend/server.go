package main

import (
    "github.com/gorilla/websocket"
    "encoding/json"
    "net/http"
    "log"
    "os"
    "sync"
    "github.com/joho/godotenv"
)

const (
    FREE = iota
    LINE
    CIRCLE
    RECT
)

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
var clients = make(map[*websocket.Conn]bool)

func echo(w http.ResponseWriter, r *http.Request) {
    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
        log.Println("Upgrade: connection failed")
        return
    }
    clients[conn] = true
    defer conn.Close()
    log.Println("client connected")

    for {
        message_type, payload, err := conn.ReadMessage()
        if err != nil {
            log.Println("ReadMessage: ", err)
            return
        }

        if message_type == websocket.CloseMessage {
            log.Println("client disconnected")
            mu.Lock()
            delete(clients, conn)
            mu.Unlock()
            return
        }

        for client := range clients {
            err = client.WriteMessage(message_type, payload)
            if err != nil {
                log.Println("WriteMessage: ", err)
                client.Close()
                mu.Lock()
                delete(clients, client)
                mu.Unlock()
                continue
            }
            log.Printf("received payload: %s", payload) 
        }
    }
}

func serialize_points(w http.ResponseWriter, r *http.Request) {
    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
        log.Println("Upgrade: connection failed")
        return
    }

    clients[conn] = true
    defer conn.Close()
    log.Println("client connected")

    for {
        message_type, payload, err := conn.ReadMessage()
        if err != nil {
            log.Println("ReadMessage: err")
            return
        }

        if message_type == websocket.CloseMessage {
            log.Println("client disconnected")
            mu.Lock()
            delete(clients, conn)
            mu.Unlock()
            return
        }
        
        // serialize draw commands
        var draw_commands []DrawCommand
        err = json.Unmarshal([]byte(payload), &draw_commands)
        if err != nil {
            log.Println("Unmarshal: ", err)
            return
        }
        broadcast_data, err := json.Marshal(draw_commands)
        if err != nil {
            log.Println("Marshal: ", err)
            return
        }
        //log.Println("-------------------------------------------")
        //log.Println(string(broadcast_data))

        for client := range clients {
            if client != conn {
                err = client.WriteMessage(message_type, broadcast_data)
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
        log.Fatal("failed to load env files", err)
    }
    port := os.Getenv("PORT")
    host := os.Getenv("HOST")
    if (port == "" || host == "") {
        log.Fatal("host or port env variables not defined")
    }

    address := host + ":" + port
    http.HandleFunc("/ws", serialize_points)

    log.Printf("server started at %v", address)

    err = http.ListenAndServe(address, nil)
    if err != nil {
        log.Fatal("ListenAndServe: ", err)
    }
}
