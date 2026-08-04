package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"
)

func main() {
	mux := http.NewServeMux()

	mux.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusOK)
		w.Write([]byte("OK"))
	})

	mux.HandleFunc("/work", func(w http.ResponseWriter, r *http.Request) {
		log.Println("Обработка запроса начата...")
		time.Sleep(10 * time.Second)
		fmt.Fprintln(w, "Работа завершена!")
		log.Println("Обработка запроса завершена")
	})

	server := &http.Server{
		Addr: ":8080",
		Handler: mux,
	}

	stop := make(chan os.Signal, 1)

	signal.Notify(stop, os.Interrupt, syscall.SIGTERM)

	go func() {
		log.Println("Сервер запущен на порту :8080")
		if err := server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("Ошибка запуска сервера: %v\n", err)
		}
	}()

	<-stop
	log.Println("Поулчен сигнал остановки. Graceful shutdown.")

	ctx, cancel := context.WithTimeout(context.Background(), 30 * time.Second)
	defer cancel()

	if err := server.Shutdown(ctx); err != nil {
		log.Fatalf("Принудительная остановка сервера: %v\n", err)
	}

	log.Println("Сервер успешно и плавно остановлен")
}