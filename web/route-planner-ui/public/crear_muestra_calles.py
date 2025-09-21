#!/usr/bin/env python3
"""
Script para crear una muestra más pequeña del dataset de calles
para desarrollo y testing
"""

import json
import random

def crear_muestra_calles(archivo_original, archivo_muestra, max_nodos=1000):
    """
    Crea una muestra más pequeña del dataset de calles
    
    Args:
        archivo_original: Archivo JSON con todas las calles
        archivo_muestra: Archivo de salida para la muestra
        max_nodos: Número máximo de nodos en la muestra
    """
    
    print(f"📖 Leyendo dataset completo: {archivo_original}")
    
    with open(archivo_original, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    print(f"📊 Dataset original:")
    print(f"   - Nodos: {len(data['nodes'])}")
    print(f"   - Aristas: {len(data['edges'])}")
    
    # Seleccionar nodos aleatorios
    nodos_originales = data['nodes']
    nodos_muestra = random.sample(nodos_originales, min(max_nodos, len(nodos_originales)))
    
    # Crear set de IDs de nodos seleccionados
    ids_nodos_muestra = {nodo['id'] for nodo in nodos_muestra}
    
    # Filtrar aristas que conecten solo nodos seleccionados
    aristas_muestra = []
    for arista in data['edges']:
        if arista['from'] in ids_nodos_muestra and arista['to'] in ids_nodos_muestra:
            aristas_muestra.append(arista)
    
    # Crear dataset de muestra
    muestra = {
        "description": f"Muestra de {max_nodos} nodos del dataset completo de calles de Culiacán",
        "nodes": nodos_muestra,
        "edges": aristas_muestra,
        "metadata": {
            "city": "Culiacán, Sinaloa, México",
            "source": "OpenStreetMap via OSMnx",
            "extracted_date": "2024-01-15",
            "total_nodes": len(nodos_muestra),
            "total_edges": len(aristas_muestra),
            "network_type": "drive",
            "sample_size": max_nodos,
            "original_nodes": len(data['nodes']),
            "original_edges": len(data['edges'])
        }
    }
    
    # Guardar muestra
    with open(archivo_muestra, 'w', encoding='utf-8') as f:
        json.dump(muestra, f, indent=2, ensure_ascii=False)
    
    print(f"💾 Muestra guardada como: {archivo_muestra}")
    print(f"📊 Estadísticas de la muestra:")
    print(f"   - Nodos: {len(nodos_muestra)}")
    print(f"   - Aristas: {len(aristas_muestra)}")
    print(f"   - Densidad: {len(aristas_muestra)/len(nodos_muestra):.2f} aristas/nodo")
    print(f"   - Reducción: {len(nodos_muestra)/len(data['nodes'])*100:.1f}% de nodos originales")

def crear_muestra_centro(archivo_original, archivo_muestra, centro_lat, centro_lon, radio_km=2):
    """
    Crea una muestra centrada en un punto específico
    
    Args:
        archivo_original: Archivo JSON con todas las calles
        archivo_muestra: Archivo de salida para la muestra
        centro_lat: Latitud del centro
        centro_lon: Longitud del centro
        radio_km: Radio en kilómetros
    """
    
    print(f"📖 Leyendo dataset completo: {archivo_original}")
    
    with open(archivo_original, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    # Función para calcular distancia
    def calcular_distancia(lat1, lon1, lat2, lon2):
        from math import radians, cos, sin, asin, sqrt
        R = 6371000  # Radio de la Tierra en metros
        dlat = radians(lat2 - lat1)
        dlon = radians(lon2 - lon1)
        a = sin(dlat/2)**2 + cos(radians(lat1)) * cos(radians(lat2)) * sin(dlon/2)**2
        c = 2 * asin(sqrt(a))
        return R * c
    
    # Filtrar nodos dentro del radio
    radio_metros = radio_km * 1000
    nodos_centro = []
    
    for nodo in data['nodes']:
        distancia = calcular_distancia(centro_lat, centro_lon, nodo['lat'], nodo['lon'])
        if distancia <= radio_metros:
            nodos_centro.append(nodo)
    
    # Crear set de IDs de nodos seleccionados
    ids_nodos_centro = {nodo['id'] for nodo in nodos_centro}
    
    # Filtrar aristas que conecten solo nodos seleccionados
    aristas_centro = []
    for arista in data['edges']:
        if arista['from'] in ids_nodos_centro and arista['to'] in ids_nodos_centro:
            aristas_centro.append(arista)
    
    # Crear dataset centrado
    muestra_centro = {
        "description": f"Muestra centrada en ({centro_lat}, {centro_lon}) con radio de {radio_km}km",
        "nodes": nodos_centro,
        "edges": aristas_centro,
        "metadata": {
            "city": "Culiacán, Sinaloa, México",
            "source": "OpenStreetMap via OSMnx",
            "extracted_date": "2024-01-15",
            "total_nodes": len(nodos_centro),
            "total_edges": len(aristas_centro),
            "network_type": "drive",
            "center_lat": centro_lat,
            "center_lon": centro_lon,
            "radius_km": radio_km,
            "original_nodes": len(data['nodes']),
            "original_edges": len(data['edges'])
        }
    }
    
    # Guardar muestra centrada
    with open(archivo_muestra, 'w', encoding='utf-8') as f:
        json.dump(muestra_centro, f, indent=2, ensure_ascii=False)
    
    print(f"💾 Muestra centrada guardada como: {archivo_muestra}")
    print(f"📊 Estadísticas de la muestra centrada:")
    print(f"   - Nodos: {len(nodos_centro)}")
    print(f"   - Aristas: {len(aristas_centro)}")
    print(f"   - Densidad: {len(aristas_centro)/len(nodos_centro):.2f} aristas/nodo")

if __name__ == "__main__":
    # Crear muestra aleatoria de 1000 nodos
    crear_muestra_calles(
        "public/calles_culiacán__sinaloa__méxico.json",
        "public/calles_culiacan_muestra_1000.json",
        max_nodos=1000
    )
    
    # Crear muestra centrada en el centro de Culiacán
    crear_muestra_centro(
        "public/calles_culiacán__sinaloa__méxico.json",
        "public/calles_culiacan_centro.json",
        centro_lat=24.7841,
        centro_lon=-107.3866,
        radio_km=2
    )
