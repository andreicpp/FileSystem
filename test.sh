#!/bin/sh

echo "-----------------[ TEST BEGIN ]------------------------"
echo "Tworzenie dysku o rozmiarze 64kB..."
./create 64 dysk
echo "Co jest co..."
./diag --help
echo "Wyswietlanie mapy dysku..."
./diag dysk
echo "Kopiowanie malego pliku na dysk..."
./cptodisk dysk small_file small1
echo "Kopiowanie drugiego malego pliku na dysk..."
./cptodisk dysk small_file small2
echo "Wyswietlanie listy plikow z dysku..."
./ls dysk
echo "Wyswietlanie mapy dysku..."
./diag dysk
echo "Usuwanie pierwszego z plikow z dysku..."
./rm dysk small1
echo "Wyswietlanie mapy dysku..."
./diag dysk
echo "Kopiowanie duzego pliku na dysk..."
./cptodisk dysk big_file big
echo "Wyswietlanie mapy dysku..."
./diag dysk
echo "Usuwanie drugiego malego pliku z dysku..."
./rm dysk small2
echo "Wyswietlanie mapy dysku..."
./diag dysk
echo "Kopiowanie duzego pliku z wirtualnego dysku..."
./cpfromdisk dysk big big_out
echo "Sprawdzanie roznicy w plikach (diff)..."
diff big_file big_out
echo "Zapelnianie dysku..."
./cptodisk dysk big_file big1
./cptodisk dysk big_file big2
./cptodisk dysk big_file big3
./cptodisk dysk big_file big4
./cptodisk dysk big_file big5
echo "Wyswietlanie mapy dysku..."
./diag dysk
echo "-------------------[TEST END]-------------------------"

