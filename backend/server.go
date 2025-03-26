package main

import (
    "github.com/gorilla/websocket"
    "net/http"
    "log"
    "flag"
    "sync"
)

var address = flag.String("addr", "localhost:3000", "http service address")
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

func main() {
    flag.Parse()
    log.SetFlags(0)
    http.HandleFunc("/ws", echo)

    log.Printf("server started at %v", *address)

    err := http.ListenAndServe(*address, nil)
    if err != nil {
        log.Fatal("ListenAndServe: ", err)
    }
}
