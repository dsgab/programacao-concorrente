package main

import (
	"fmt"
	"math"
)

func main() {
	var maiorNumero, numeroGoroutines, totalPrimos int
	fmt.Print("Digite até qual número deve ser verificado se é primo: ")
	fmt.Scan(&maiorNumero)
	fmt.Print("Digite qual o número de goroutines que devem ser utilizadas: ")
	fmt.Scan(&numeroGoroutines)
	fmt.Println("N =", maiorNumero, "| M = ", numeroGoroutines)

	numeros := make(chan int, numeroGoroutines) // Canal para enviar todos os números dentro do intervalor desejado para serem testados
	primos := make(chan int, numeroGoroutines)  // Canal para receber a quantidade de números primos encontrados pro cada goroutine

	for i := 0; i < numeroGoroutines; i++ {
		go func(n int) {
			quantidadePrimos := 0
			totalNumeros := 0
			for {
				numero, continua := <-numeros
				if continua {
					if ehPrimo(numero) {
						quantidadePrimos++
					}
					totalNumeros++
				} else {
					fmt.Println("Goroutine", i, "finalizada. Primos encontrados:", quantidadePrimos, "- Números verificados:", totalNumeros)
					primos <- quantidadePrimos
					return
				}
			}
		}(i)
	}

	// Envia todos os números do intervalo desejado para o canal "numeros"
	for i := 1; i <= maiorNumero; i++ {
		numeros <- i
	}
	close(numeros) // Fecha o canal "numeros" já que não há mais números para serem enviados
	fmt.Println("Todos os números foram enviados.")

	for i := 0; i < numeroGoroutines; i++ {
		totalPrimos += <-primos
		fmt.Println(i, "goroutine(s) recebida(s).")
	}
	fmt.Println("O número total de primos no intervalo é:", totalPrimos)
	close(primos)
}

func ehPrimo(n int) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	for i := 3; i < int(math.Sqrt(float64(n)))+1; i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}
