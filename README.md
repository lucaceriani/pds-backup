<img align="center" src="logo.png"></img>

# Specifiche
Il progetto consiste nella creazione di un sistema client-server che effettua un backup incrementale del contenuto di una cartella (del client) in una cartella remota (del server).
Il client dovrà essere eseguito come un **servizio di background** e deve mantenere in sync la cartella specificata con quella remota.
Appena si **effettua una modifica** (rinomina, modifica, creazione, cancellazione) alla cartella monitorata il **server deve essere aggiornato**.

Nel caso di errori nella comunicazione tra server e client, il sistema continuerà a monitorare la cartella e proverà a sincronizzarsi di nuovo non appena il server sarà di nuovo disponibile.

La connessione sarà di tipo **TCP/IP**.

# Scelte progettuali
Quale **protocollo di rete** per le comunicazioni tra client e server?
> La soluzione più semplice è stata quella di adottare un approccio basato su codifica **ASCII** per la semplicità e la trasparenza nel debug. Le **specifiche del protocollo** sono descritte in seguito.

La comunicazione **è sicura**?
> Boost.Asio supporta il **protocollo TLS** verrà quindi utilizzato per la creazione di un canale sicuro.

Come avviene l'**autenticazione** del client?
> Il client invia, attraverso una canale TLS, nome utente e password.

Come si deve comportare il software in una condizione di **cold start**, ovvero quando i file all'interno della cartella da monitorare sono stati modificati senza che l'applicazione fosse in esecuzione?
> All'avvio, il programma effettuerà **una scansione completa di tutti i file** e, per ognuno di questi, contorllerà che sia già presenta sul server, in caso contrario verrà effettuato un upload. Questo caso è **equiparabile all'avvio del programma per la prima volta** e selezionando come cartella da sincronizzare una cartella non vuota.

Come si gestisce la **struttura dei file** in memoria?
> Si implementa in modo identico tra client e server, mediante una libreria condivisa.

Chi è la **SOOT**?
> Il **client** è la Single Source Of Truth in quanto è lui che si occupa di sapere quali file sono sincronizzati e di effettuarne l'upload in caso non lo siano .


# Protocollo

## Header
L'header è lungo **48 byte**. La codifica dell'header è di tipo ASCII, non è binario.

#### [0-3] Versione
I **primi 4 byte** indicano la versione del protocollo, di cui i primi due la versione maggiore e gli 
ultimi due la versione minore. 

> Ad esempio `0123` rappresenta la versione 1.23 e `0100` rappresenta la versione 1.00.

#### [4-7] Codice messaggio

I **successivi 4 byte** rappresentano il tipo di messaggio scambiato.
Il primo byte è sempre una `M` maiuscola, i successivi 3 sono il codice del messaggio, che molto vagamente vogliono ricordare i codici HTTP.

#### [8-31] ID di sessione

I **successivi 24 byte** sono l'ID della sessione utilizzato come cookie per il riconoscimento di un utente già loggato. I caratteri ASCII utilizzati sono quelli della codifica `base64url`, ovvero tutte le lettere dell'alfabeto, maiuscole e minuscole, le dieci cifre e i caratteri `-` e `_`.

Così facendo ci si allinea quantomeno alle *best practices* dell'OWASP, consultabili a [questo link](https://cheatsheetseries.owasp.org/cheatsheets/Session_Management_Cheat_Sheet.html#session-id-length) in cui si consiglia di utilizzare al minimo una lunghezza pari a 128bit (64 bit di entropia), ovvero con 2^128 ~ 10^38 possibili combinazioni.

In questo protocollo si usano 24 caratteri con 64 combinazioni ciascuno, ovvero 64^24 ~ 10^43.

#### [32-47] Lunghezza del body

Gli  **ultimi 16 byte** sono la codifica ASCII del numero, in base 10, di byte di lunghezza del body.

## Body
Il body può essere di lunghezza variabile, ogni campo del body è codificato come ASCII a esclusione della parte file del  `021 - upload file` in cui il file stesso è inviato *raw*.

Le virgole `,` nelle tabelle successive devono essere interpretate come carattere nullo `0x00`.

## Codici del messaggio

### Client

| codice | significato | body |
|------- | ----------- | ---- |
| 010 | richiesta login | (vuoto) |
| 011 | invio credenziali | username, password |
| 020 | probe file | percorso file, checksum |
| 021 | upload file | percorso file, raw file |
| 030 | cancella file | percorso file |
| 031 | cancella cartella | percorso cartella |

### Server

| codice | significato | body |
|------- | ----------- | ---- |
| 100 | ok / procedi | (vuoto) |
| 200 | errore generico server | (vuoto) |
| 201 | credenziali non valide / non autenticato | (vuoto) |
| 203 | impossibile caricare il file (es. spazio esaurito) | percorso file |
| 204 | file non presente / checksum non corrispondente | percorso file |
| 210 | errore client / trasmissione errata / header errato | (vuoto) |
| 211 | versione protocollo incompatibile | (vuoto)

## Esempio
Dopo essermi autenticato, ho ottenuto l'ID di sessione `TkZ1Q-ILbN25UKGM5BUVzl_7`, voglio inviare un file da **20 byte** che si trova, nel client, nella cartella **prova/abc.txt**. I byte totali del body saranno quindi 20 (file) + 13 (percorso file) + 1 (carattere nullo) = **34 byte**.

**Il messaggio**:

in cui `~` rappresenta il caratter nullo e `*` rappresentano i byte del file.
```
           header
0001 M021   | 0
TkZ1 Q-IL   | 8   << id sessione
bN25 UKGM   |16 
5BUV zl_7   |24
0000 0000   |32   << lunghezza body
0000 0034   |40
            |
           body
prov a/ab   |48
c.tx t~**   |
**** ****   |
**** ****   |
**          |
```

