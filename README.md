# Progetto PdS - Backup

## Definizione del protocollo

### Header
L'header è lungo 40 byte.
La codifica dell'header è di tipo ASCII, non è binario.

#### Versione

I primi 4 byte indicano la versione del protocollo, di cui i primi due la versione maggiore e gli 
ultimi due la versione minore. 

Ad esempio `0123` rappresenta la versione 1.23 e `0100` rappresenta la versione 1.00.


#### Tipo di messaggio
I successivi 4 byte rappresentano il tipo di messaggio scambiato.

Il primo byte è sempre una M maiuscola, i successivi 3 sono il codice del messaggio, che molto vagamente vogliono ricordare i codici HTTP.

#### Lunghezza body
I successivi 16 byte sono la codifica ASCII del numero, in base 10, di byte di lunghezza del body.

### Body
Il body può essere di lunghezza variabile.
