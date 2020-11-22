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
L'header è lungo 40 byte. La codifica dell'header è di tipo ASCII, non è binario.

I **primi 4 byte** indicano la versione del protocollo, di cui i primi due la versione maggiore e gli 
ultimi due la versione minore. 

> Ad esempio `0123` rappresenta la versione 1.23 e `0100` rappresenta la versione 1.00.

I **successivi 4 byte** rappresentano il tipo di messaggio scambiato.
Il primo byte è sempre una M maiuscola, i successivi 3 sono il codice del messaggio, che molto vagamente vogliono ricordare i codici HTTP.

I **successivi 16 byte** sono il codice utente utilizzato come cookie per la connessione. I caratteri ASCII utilizzati sono tutti quelli stampabili escluso lo spazio: da `! = 0x21` fino a `~ = 0x7e`. Si creano un questo modo 94^16, ovvero circa 10^30 combinazioni.

Gli  **ultimi 16 byte** sono la codifica ASCII del numero, in base 10, di byte di lunghezza del body.

## Body
Il body può essere di lunghezza variabile, ogni campo del body è codificato come ASCII a esclusione della parte file del  `021 - upload file` in cui il file stesso è inviato *raw*.

Le virgole nelle tabelle successive devono essere interpretate come carattere nullo `0x00`.

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
Dopo essermi autenticato, voglio inviare un file da **20 byte** che si trova, nel client, nella cartella **prova/abc.txt**. I byte totali saranno quindi 20 (file) + 13 (percorso file) + 1 (carattere nullo) = **34 byte**.

**Il messaggio**:

in cui `~` rappresenta il caratter nullo e `*` rappresentano i byte del file.
```
           header
0100 M021    | 
aFr! hJ^8    | < user code
us3r c0de    | 
0000 0000    | < body lenght
0000 0034    |
             V

            body
prov a/ab    |
c.tx t~**    |
**** ****    |
**** ****    |
**           |
```

