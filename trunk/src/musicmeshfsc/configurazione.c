const char* TABLES[] = { "artista", "musica", "file" };
const int TABLES_SIZE = 3;

const char* KEYWORDS[] = { "artist", "title", "album", "track", "genre", "year",
        "host", "path", "type" };
const char* COLUMNS[] = { "artista.nome_artista", "musica.titolo",
        "musica.nome_album", "musica.traccia", "musica.genere",
        "musica.lavoro_anno", "file.host", "file.path", "file.formato" };
const int KEYWORDS_SIZE = 9;

const char* COLUMN_EXT = "file.formato";

const char* JOINS[] = { "artista,musica", "musica,file" }; // senza spazi!!!
const char* WHERE[] = {
    "(artista.nome_artista = musica.artista_nome_artista)",
    "(musica.titolo = file.musica_titolo) AND (musica.nome_album = "
            "file.musica_nome_album) AND (musica.traccia = file.musica_traccia)"
};

const int JOINS_SIZE = 2;
