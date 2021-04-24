#!/bin/bash
#Création fichier .c et .h
echo "Compilation de tpc"
make
echo -e "\n\n"

#Initialisation des variables de chemins
exec="tpcc"
repository="./test"
good_rep=$repository"/good"
syn_rep=$repository"/syn-err"
sem_rep=$repository"/sem-err"
results=$repository"/resultat.log"
rm -f $results #Suppression anciens résultats

#Tests des positifs
countGood=0
countSyn=0
countSem=0
echo "====Début des tests positifs===="
for i in $(ls $good_rep)
do
	echo "./bin/$exec < $i"
  	./bin/$exec < $good_rep/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
	ret=$?
  	echo "Résultat attendus pour $i : 0, résultat obtenu: $ret" >> $results
	if [ $ret == 0 ]; then
		countGood=$((countGood + 1))
	fi
done

#Tests des négatifs
echo -e "\n\n"
echo "====Début des tests d'erreurs lexicales / syntaxiques===="
for i in $(ls $syn_rep)
do
	echo "./bin/$exec < $i"
    	./bin/$exec < $syn_rep/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
	ret=$?
  	echo "Résultat attendus pour $i: 1, résultat obtenu: $ret" >> $results
	if [ $ret == 1 ]; then
		countSyn=$((countSyn + 1))
	fi
done

echo -e "\n\n"
echo "====Début des tests d'erreurs lexicales / syntaxiques===="
for i in $(ls $sem_rep)
do
	echo "./bin/$exec < $i"
    	./bin/$exec < $sem_rep/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
	ret=$?
  	echo "Résultat attendus pour $i: 2, résultat obtenu: $ret" >> $results
	if [ $ret == 2 ]; then
		countSem=$((countSem + 1))
	fi
done


echo -e "\n\n"

echo "Les Résultats des tests sont disponible dans le fichier $results"
nb=$(ls $good_rep | wc -l)
echo "$countGood / $nb tests positifs sont un succés"
nb=$(ls $syn_rep | wc -l)
echo "$countSyn / $nb tests erreurs lexicales / syntaxiques sont un succés"
nb=$(ls $sem_rep | wc -l)
echo "$countSem / $nb tests erreurs sémantiques sont un succés"

echo -e "\n\n"
