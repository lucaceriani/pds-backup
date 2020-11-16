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

## Versione 1.00

### Client - Codici messaggio 

| codice | significato |
|-|-|
| 010 | richiesta login |
| 011 | invio credenziali |
| 020 | probe file |
| 021 | upload file |
| 030 | cancella file |
| 031 | cancella cartella |

### Server - Codici messaggio

| codice | significato |
|-|-|
| 100 | ok / procedi |
| 200 | errore generico server |
| 201 | credenziali non valide |
| 203 | impossibile caricare il file (es. spazio esaurito)  |
| 204 | file non presente / checksum non corrispondente |
| 210 | errore client / trasmissione errata / header errato |
| 211 | versione protocollo incompatibile |


 

