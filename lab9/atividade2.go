//introduz canais na aplicação Go
package main

import (
	"fmt"
)

func tarefa(str chan string) {
	var msg = ""
	msg = <-str
	fmt.Print(msg)
	str <-"Oi Main, bom dia, tudo bem?\n"
	msg = <-str
	fmt.Print(msg)
	str <-"Certo, entendido.\n"
	msg = <-str
	fmt.Print("finalizando\n")
	str <- ""
}

func main() {
	var msg = "" //string vazia
	
	//cria um canal de comunicacao nao-bufferizado
	str := make(chan string)
	
	//cria um goroutine que executará a função 'tarefa'
	go tarefa(str)

	str <-"Olá, Goroutine, bom dia!\n"
	msg = <-str
	fmt.Print(msg)
	str <-"Tudo bem! Vou terminar tá?\n"
	msg = <-str
	fmt.Print(msg)
	str <-""
	msg = <-str
	fmt.Print("finalizando\n")
}
