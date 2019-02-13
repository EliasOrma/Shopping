#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream> //tiedostonluku
#include <algorithm> // sort
#include <iomanip> //2-desimaalintarkkuus tulostuksessa



using namespace std;

// tuotteiden tallentamiseen tarkoitettu tietorakenne
struct tieto_pari {
    string nimi;
    double hinta;
};

// Tietyn tuotteen etsimiseen rakenteesta vector < tieto_pari >.
struct find_tietopari
{
    string nimi;
    find_tietopari(string nimi) : nimi(nimi) {}
    bool operator () ( const tieto_pari& m ) const
    {
        return m.nimi == nimi;
    }
};

// tietorakenteen nimeäminen, alatietorakeenteen nimeäminen
typedef map < string, map < string, vector <tieto_pari> > > paakauppalista;
typedef map < string, vector < tieto_pari > > subdict;

// funktio jolla sort järjestää tietopari_1 < tietopari_2 tilanteen tuotteen nimen mukaan.
bool compare_by_word(const tieto_pari& lhs, const tieto_pari& rhs) {
    return lhs.nimi < rhs.nimi;
}


// split funktio otettu 5.2.2 koodipohjasta
vector<string> split(const string& s, const char delimiter, bool ignore_empty = false){

    vector<string> result;
    string tmp = s;


    while(tmp.find(delimiter) != string::npos)
    {
        string new_part = tmp.substr(0, tmp.find(delimiter));
        tmp = tmp.substr(tmp.find(delimiter)+1, tmp.size());
        if(not (ignore_empty and new_part.empty()))
        {
            result.push_back(new_part);
        }
    }
    if(not (ignore_empty and tmp.empty()))
    {
        result.push_back(tmp);
    }
    return result;
}


paakauppalista lue_tiedosto(string tiedoston_nimi) {
    /* Funktio lukee käyttäjän määrittelemän tiedoston ja tallentaa informaation
     * päätietorakenteeseen, jos tulee virhe, funktio palauttaa tyhjän tietorakenteen
     * :param: avattavan tiedoston nimi
     * :return: paakauppalista
     */
    paakauppalista kauppa_kirjasto;
    ifstream tiedosto_olio(tiedoston_nimi);
    if (not tiedosto_olio) {
        cout << "Error: the input file cannot be opened" << endl;
        return kauppa_kirjasto;
    }
    else {
        string rivi = "";

        // Käydään rivitläpi tiedostosta kunnes ne loppuu.
        while(getline(tiedosto_olio, rivi)) {

            vector <string> palat = split(rivi,';');
            //jos rivejä on 4
            if (palat.size() == 4 and palat.at(0) != "" and palat.at(1) != "" and palat.at(2) != "" and palat.at(3) != "") {
                string kaupan_nimi = palat.at(0);
                string sijainti = palat.at(1);
                string tuotteen_nimi = palat.at(2);

                // Tässä oletetaan että, kauppa ei myy mitää tuotetta hinnalla 0.
                // eli out-of-stock == 0.
                double tuotteen_hinta = 0;

                if (palat.at(3) != "out-of-stock") {
                    tuotteen_hinta = stod(palat.at(3));
                }


                vector < tieto_pari > tavarat;
                subdict tietty_kauppa;

                tieto_pari tuote = { tuotteen_nimi, tuotteen_hinta };

                // luodaan tyhjä tietorakenne tietylle kauppaketjulle, jos tämä ei jo kuulu
                // tietorakenteeseen
                if (kauppa_kirjasto.find(kaupan_nimi)==kauppa_kirjasto.end()) {
                    kauppa_kirjasto.insert( {kaupan_nimi , tietty_kauppa});
                }

                // luodaan tyhjä tietorakenne tietylle kaupalle, jos tämä ei kuulu jo tieto-
                // rakenteeseen
                if (kauppa_kirjasto[kaupan_nimi].find(sijainti) == kauppa_kirjasto[kaupan_nimi].end()) {
                    kauppa_kirjasto[kaupan_nimi].insert({sijainti, tavarat});
                }
  
                // Iteraattori
                auto& kaupan_tuotelista = kauppa_kirjasto[kaupan_nimi][sijainti];

                // Iteroi kaupantuotelistan läpi käyttämällä nimi arvoa, ja palauttaa sen paikan jos se löytyy jo listasta
                auto it = find_if(kaupan_tuotelista.begin(),kaupan_tuotelista.end(), find_tietopari(tuotteen_nimi));

                if( it != kaupan_tuotelista.end()) {
                    // Poistetaan listasta tavaran vanhat tiedot, seuraavaksi listätään se uudestaa viimeisimmällä hinnalla
                    kaupan_tuotelista.erase(it);
                }

                //  lisätään tuote tietyn kauppaketjun tietyn kaupan tuotevalikoimaan
                kauppa_kirjasto[kaupan_nimi][sijainti].push_back(tuote);

                }
            else {
                cout << "Error: the file has an erroneous line" << endl;
                kauppa_kirjasto.clear();
                return kauppa_kirjasto;
            }
        }


        }
        tiedosto_olio.close();
        return kauppa_kirjasto;

}


void stores(paakauppalista kirjasto, string chain) {
    /* Funktio käy läpi tietyn kauppaketjun jokaisen kaupan sijainnin ja tulostaa ne.
     * Jos kauppaketjua ei löydy funktio tulostaa virhe ilmoituksen.
     * :param: päätietorakenne kirjasto, kauppaketjun nimi
     */
    // selvittää onko kauppaketju kirjastossa
    if (kirjasto.find(chain) != kirjasto.end()) {

        auto stores = kirjasto[chain];
        // Käy läpi jokaisen kaupan sijainnin ja tulostaa ne.
        for (auto sub_iter = stores.begin(); sub_iter != stores.end(); ++sub_iter) {
            cout << sub_iter->first << endl;
        }
        return;
    }
    else {
           cout << "Error: an unknown chain" << endl;
           return;
    }
}

void selection(paakauppalista kirjasto,string chain, string sijainti) {
    /* Funktio käyläpi tietyn kauppaketjun tietyn kaupan tuotteet
     * ja tulostaa niiden nimet ja niiden hinnat. Jos kauppaketjua ei lyödy tulostetaan
     * virheilmoitus. Jos tiettyä kauppaa ei löydy tulostetaan virhe ilmoitus.
     * :param: päätietorakenne kirjasto, kauppaketjun nimi, kauppaketjun sijainti
     */

    // selvittää onko kauppaketju kirjastossa
    if (kirjasto.find(chain) != kirjasto.end()) {
        // selvittää onko tiettykauppa tietyn kauppaketjun kohdalla kirjastossa
        if(kirjasto[chain].find(sijainti) != kirjasto[chain].end()) {

            // järjestetään tietyn kaupan tuotteet aakkosjärjestykseen
            sort(kirjasto[chain][sijainti].begin(), kirjasto[chain][sijainti].end(), compare_by_word);

            // käy läpi tietyn kaupan tuotteet
            for (auto i = kirjasto[chain][sijainti].begin(); i != kirjasto[chain][sijainti].end(); i++) {
                cout << i->nimi << " ";
                // jos hinta on 0 tulostetaan out of stock.
                if (i->hinta== 0) {
                    cout << "out of stock" << endl;
                }
                // muuten tulostetaan hinta
                else {
                    cout  << i -> hinta << endl;
                }
            }
        }
        else {
            cout << "Error: unknown store" << endl;
        }
    }

    else {
           cout << "Error: unknown chain" << endl;
    }
}

void cheapest(paakauppalista kirjasto,string tavara) {
    /* Funktio käy läpi kaikkien kauppaketjujen kaikkien kauppojen tuotteet ja etsii halvimman/halvimmat
     * hinnan/hinnat tuotteelle ja tulostaa halvimmalla myyvien kauppojen ketjut ja sijainnit.
     * Jos tuotetta ei löydy tulostetaan virheilmoitus.
     * Jos tuote löytyy mutta se on tilapäisesti loppu joka kaupasta tulostetaan virheilmoitus.
     * :param: päätietorakenne kirjasto, tavaran nimi
     */

    double cheapest_price = 0;
    vector <string> cheapest_stores;
    int out_of_stock_laskuri = 0;

    // Käy läpi kauppaketjut
    for (auto i = kirjasto.begin(); i!= kirjasto.end(); i++) {

        // Käy läpi tietyt kaupat
        for (auto sub_i = kirjasto[i->first].begin(); sub_i != kirjasto[i->first].end(); sub_i++) {

            // Käy läpi tuotteet
            for (auto tuote_i = kirjasto[i->first][sub_i->first].begin();tuote_i != kirjasto[i->first][sub_i->first].end(); tuote_i ++) {
                string tuote = tuote_i->nimi;
                double hinta = tuote_i->hinta;

                if (tuote == tavara) {
                    // Jos tuotteen hinnaksi on merkitty 0, se on out of stock
                    if(hinta== 0) {
                        ++out_of_stock_laskuri;
                    }
                    // Jos tuotteen hinta ei ole 0 ja cheapest price on "outofstock"
                    // muutetaan tuotteen hinta halvimmaksi hinnaksi.
                    else if (cheapest_price == 0 and hinta > double(0.0)) {
                        cheapest_price = hinta;

                    }

                    /*  Jos tuote on halvempi kuin aikaisempi tuotteen hinta muutetaan
                        Sen arvo halvimmaksi hinnaksi, tyhjennetään cheapest_stores-lista
                        Ja asetetaan tarkasteltavana oleva kauppa kyseisen listan ainoaksi
                        Jäseneksi.
                        Huom! Funktio ei pääse tänne asti, jos edellinen tai ensimmäinen ehto toteutuu.
                        Näin vältämme olemassa olevan tuotteen hinnan vertailua outofstock hintaan( 0 )
                        Eli funktio tulee seuraavaan ehtoon vasta kun cheapest price on jokin muu kuin
                        out of stock eli ( 0 )*/
                    else if(hinta < cheapest_price) {
                        cheapest_price = hinta;
                        cheapest_stores.clear();
                        cheapest_stores.push_back(i->first+" "+sub_i->first);
                    }

                    // Jos hinta on sama kuin halvin hinta, lisätään tarkaseltava kauppa
                    // cheapest_stores listaan
                    else if (hinta == cheapest_price) {
                        cheapest_stores.push_back(i->first+" "+sub_i->first);
                    }
                }
            }
        }
    }

    // Jos cheapest price on 0 eli out of stock ja käytiin läpi out of stock- statuksella
    // esiintyviä tuotteita tulostetaan seuraava.
    if ( cheapest_price == 0 and out_of_stock_laskuri != 0) {
        cout << "The product is temporarily out of stock everywhere." << endl;
        return;
    }

    // Jos kyseistä tuotetta ei löydetty.
    else if (cheapest_price == 0) {
        cout << "Product is not part of product selection." << endl;
        return;
    }
    // tulostetaan halvin hinta ja kauppa/kaupat.
    else {
        cout << cheapest_price << " euros" << endl;
        for(auto i = cheapest_stores.begin(); i != cheapest_stores.end(); i++) {
            cout  << *i << endl;
        }
        return;
    }

}

void products(paakauppalista kirjasto) {
    /* Funktio käy läpi jokaisen kaupan tuotteen, tekee niistä listan, järjsetää ne
     * aakkkosjärjestykseen ja tulostaa ne.
     * :param: päätietorakenne kirjasto
     */
    vector <string> tuote_lista;
    vector <string>::iterator iter;

    // Käy läpi kauppaketjut
    for (auto i = kirjasto.begin(); i!= kirjasto.end(); i++) {

        // Käy läpi tietyt kaupat
        for (auto sub_i = kirjasto[i->first].begin(); sub_i != kirjasto[i->first].end(); sub_i++) {

            // Käy läpi tuotteet
            for (auto tuote_i = kirjasto[i->first][sub_i->first].begin();tuote_i != kirjasto[i->first][sub_i->first].end(); tuote_i ++) {
                string tuote = tuote_i->nimi;

                iter = find (tuote_lista.begin(),tuote_lista.end(), tuote);

                // Jos tuotetta ei löytynyt listasta lisätään se.
                if(iter == tuote_lista.end()) {
                    tuote_lista.push_back(tuote);
                }
            }
        }
    }
    // Järjestetään lista aakkosjärjestykseen.
    sort(tuote_lista.begin(),tuote_lista.end());

    for (auto i : tuote_lista) {
        cout << i << endl;
    }
}

void error(string toiminto) {
    /* Funktio tulostaa tiedon virheellisestä yrityksestä
     * käyttää jotakin toimintoa
     * :param: toiminnon nimi.
     */
    cout << "Error: error in command " << toiminto << endl;
}

int main()
{
    string tiedoston_nimi = "";
    cout << "Input file: ";
    getline(cin,tiedoston_nimi);

    paakauppalista kirjasto = lue_tiedosto(tiedoston_nimi);
    // Jos kirjasto on tyhjä, tiedoston luku epäonnistu ja ohjelma suljetaan.
    if (kirjasto.empty()) {
        return EXIT_FAILURE;
    }

    // Asetetaan cout tulostamaan 2 numeron tarkkuudella.
    cout << fixed;
    cout << setprecision(2);

    // Käyttöliittymän alustus
    string rivi = "";
    vector< string > rivi_lista;

    cout  << "> ";
    getline(cin,rivi);

    rivi_lista = split(rivi, ' ');
    string toiminto = rivi_lista[0];

    // Käyttöliittymä
    while(toiminto != "quit") {

        if (toiminto == "chains") {

            if (rivi_lista.size() == 1) {
            // Käy läpi jokaisen kauppaketjun ja tulostaa ne.
                for (auto iter = kirjasto.begin(); iter != kirjasto.end(); ++iter) {
                    cout << iter->first << endl;
                }
            }
            else {
                error(toiminto);
            }
        }


        else if (toiminto == "stores") {
            if (rivi_lista.size()==2) {
                string chain = rivi_lista[1];
                stores(kirjasto,chain);
            }
            else {
                error(toiminto);
            }
        }
        else if (toiminto == "selection") {
            if (rivi_lista.size()==3) {
                string chain = rivi_lista[1];
                string sijainti = rivi_lista[2];
                selection(kirjasto,chain,sijainti);
            }
            else {
                error(toiminto);
            }
        }
        else if (toiminto == "cheapest") {
            if (rivi_lista.size()==2) {
                string tavara = rivi_lista[1];
                cheapest(kirjasto,tavara);
            }
            else {
                error(toiminto);
            }
        }
        else if (toiminto == "products") {
            if(rivi_lista.size()==1) {
                products(kirjasto);
            }
            else {
                error(toiminto);
            }
        }
        else {
            cout << "Error: unknown command " << endl;
        }
        // Jokaisen whileloop kierroksen päätyttyä tulostetaan komentosyöte.
        cout << "> ";
        getline(cin,rivi);
        rivi_lista = split(rivi, ' ');
        toiminto = rivi_lista[0];
    }
    return EXIT_SUCCESS;
}
