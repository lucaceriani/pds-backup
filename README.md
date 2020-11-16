<img align="center" src="logo.png"></img>

## Protocollo

### Header
L'header è lungo 40 byte. La codifica dell'header è di tipo ASCII, non è binario.

I **primi 4 byte** indicano la versione del protocollo, di cui i primi due la versione maggiore e gli 
ultimi due la versione minore. 

> Ad esempio `0123` rappresenta la versione 1.23 e `0100` rappresenta la versione 1.00.

I **successivi 4 byte** rappresentano il tipo di messaggio scambiato.
Il primo byte è sempre una M maiuscola, i successivi 3 sono il codice del messaggio, che molto vagamente vogliono ricordare i codici HTTP.

I **successivi 16 byte** sono la codifica ASCII del numero, in base 10, di byte di lunghezza del body.

### Body
Il body può essere di lunghezza variabile, ogni campo del body è codificato come ASCII a esclusione della parte file del  `021 - upload file` in cui il file stesso è inviato *raw*.

Le virgole nelle tabelle successive devono essere interpretate come carattere nullo `0x00`.

### Esempio
Dopo essermi autenticato, voglio inviare un file da **123 byte** che si trova, nel client, nella cartella **./prova/abc.txt**.

Il messaggio:
```
0100 M021    |
0000 0000    |
0000 0123    |__ header

prov a/ab    |
c.tx t **    |
*rawfile*    |
*********    |__ body
```


## Versione 1.00

### Client - Codici messaggio 

| codice | significato | body |
|------- | ----------- | ---- |
| 010 | richiesta login | (vuoto) |
| 011 | invio credenziali | username, password |
| 020 | probe file | percorso file, checksum |
| 021 | upload file | percorso file, raw file |
| 030 | cancella file | percorso file |
| 031 | cancella cartella | percorso cartella |

### Server - Codici messaggio

| codice | significato |
|------- | ----------- |
| 100 | ok / procedi |
| 200 | errore generico server |
| 201 | credenziali non valide / non autenticato |
| 203 | impossibile caricare il file (es. spazio esaurito)  |
| 204 | file non presente / checksum non corrispondente |
| 210 | errore client / trasmissione errata / header errato |
| 211 | versione protocollo incompatibile |
