DROP TABLE IF EXISTS artista;
CREATE TABLE artista(
    nome_artista VARCHAR NOT NULL PRIMARY KEY DEFAULT 'no_artist'
);

DROP TABLE IF EXISTS musica;
CREATE TABLE musica(
    titolo VARCHAR NOT NULL DEFAULT 'no_title',
    nome_album VARCHAR NOT NULL DEFAULT 'no_album',
    traccia VARCHAR NOT NULL DEFAULT '0',
    genere VARCHAR NOT NULL DEFAULT 'no_genre',
    artista_nome_artista VARCHAR NOT NULL DEFAULT 'no_artist' CONSTRAINT
            fk_musica_artista REFERENCES artista(nome_artista) ON DELETE CASCADE,
    lavoro_anno VARCHAR NOT NULL DEFAULT '0',
    PRIMARY KEY(titolo, nome_album, traccia)
);

DROP TABLE IF EXISTS file;
CREATE TABLE file(
    host VARCHAR NOT NULL DEFAULT '127.0.0.1',
    path VARCHAR NOT NULL,
    permessi INT NOT NULL DEFAULT 4, -- read-only
    formato VARCHAR NOT NULL DEFAULT 'mp3',
    dimensioni INT NOT NULL DEFAULT 0,  -- dimensioni del file originale
    data_ultimo_aggiornamento TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    musica_titolo VARCHAR NOT NULL DEFAULT 'no_title',
    musica_nome_album VARCHAR NOT NULL DEFAULT 'no_album',
    musica_traccia VARCHAR NOT NULL DEFAULT '0',
    basename VARCHAR NOT NULL,
    PRIMARY KEY(host, path),
    FOREIGN KEY(musica_titolo, musica_nome_album, musica_traccia)
            REFERENCES musica(titolo, nome_album, traccia) ON DELETE CASCADE
);

-- Se creo una chiave esterna, deve esistere il valore referenziato
CREATE TRIGGER IF NOT EXISTS fk_musica_artista_insert BEFORE INSERT ON musica
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'fk_musica_artista_insert')
    WHERE (SELECT nome_artista FROM artista WHERE (NEW.artista_nome_artista IS
            NULL OR nome_artista = NEW.artista_nome_artista)) IS NULL;
END;

CREATE TRIGGER IF NOT EXISTS fk_file_musica_insert BEFORE INSERT ON file
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'fk_file_musica_insert')
    WHERE (SELECT titolo FROM musica WHERE
            (NEW.musica_titolo IS NULL OR titolo = NEW.musica_titolo) AND
            (NEW.musica_nome_album IS NULL OR nome_album = NEW.musica_nome_album) AND
            (NEW.musica_traccia IS NULL OR traccia = NEW.musica_traccia))
        IS NULL;
END;

-- Se aggiorno una chiave esterna, deve esistere il valore referenziato
CREATE TRIGGER IF NOT EXISTS fk_musica_artista_update BEFORE UPDATE ON musica
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'fk_musica_artista_update')
    WHERE (SELECT nome_artista FROM artista WHERE (NEW.artista_nome_artista IS
            NULL OR nome_artista = NEW.artista_nome_artista)) IS NULL;
END;

CREATE TRIGGER IF NOT EXISTS fk_file_musica_update BEFORE UPDATE ON file
FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'fk_file_musica_update')
    WHERE (SELECT titolo FROM musica WHERE titolo = NEW.musica_titolo AND
            nome_album = NEW.musica_nome_album AND traccia = NEW.musica_traccia)
            IS NULL;
END;

-- Se cancello un valore referenziato cancella anche le chiavi esterne
CREATE TRIGGER IF NOT EXISTS fk_musica_artista_delete BEFORE DELETE ON artista
FOR EACH ROW BEGIN
    DELETE FROM musica WHERE artista_nome_artista = OLD.nome_artista;
END;

CREATE TRIGGER IF NOT EXISTS fk_file_musica_delete BEFORE DELETE ON musica
FOR EACH ROW BEGIN
    DELETE FROM file WHERE musica_titolo = OLD.titolo AND
            musica_nome_album = OLD.nome_album AND musica_traccia = OLD.traccia;
END;

-- -- Vista che riunisce tutto come se fosse una sola tabella (read-only!)
-- DROP VIEW IF EXISTS vw_all_in_one;
-- CREATE VIEW vw_all_in_one AS
--     SELECT file.host AS host, file.path AS path, file.permessi AS permessi,
--             file.formato AS formato, file.dimensioni AS dimensioni,
--             file.data_ultimo_aggiornamento AS data_ultimo_aggiornamento,
--             musica.titolo AS titolo, musica.nome_album AS album, musica_traccia
--             AS traccia, musica.genere AS genere, artista.nome_artista AS
--             artista, musica.lavoro_anno AS anno, file.basename AS basename
--     FROM file, musica, artista
--     WHERE (file.musica_titolo = musica.titolo AND
--             file.musica_nome_album = musica.nome_album AND
--             file.musica_traccia = musica.traccia AND
--             musica.artista_nome_artista = artista.nome_artista);
