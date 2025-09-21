#!/usr/bin/env python3
"""
Script para extraer calles reales de OpenStreetMap
Usa OSMnx para obtener la red de calles de una ciudad
"""

import osmnx as ox
import json
import networkx as nx

def extraer_calles_ciudad(ciudad="Culiacán, Sinaloa, México", distancia_km=5):
    """
    Extrae la red de calles de una ciudad usando OpenStreetMap
    
    Args:
        ciudad: Nombre de la ciudad
        distancia_km: Radio de búsqueda en kilómetros
    """
    
    print(f"🗺️ Extrayendo calles de {ciudad}...")
    
    # Configurar OSMnx (versión más reciente)
    # ox.config(use_cache=True, log_console=True)  # Deprecated en versiones recientes
    
    # Obtener la red de calles
    G = ox.graph_from_place(ciudad, network_type='drive')
    
    # Simplificar la red (opcional) - solo si no está ya simplificada
    try:
        G = ox.simplify_graph(G)
    except Exception as e:
        print(f"⚠️ El grafo ya está simplificado: {e}")
        pass
    
    print(f"✅ Red extraída: {len(G.nodes)} nodos, {len(G.edges)} aristas")
    
    # Convertir a formato JSON
    nodes = []
    edges = []
    
    # Procesar nodos
    for node_id, data in G.nodes(data=True):
        nodes.append({
            "id": node_id,
            "lat": data['y'],
            "lon": data['x'],
            "elevation": data.get('elevation'),
            "type": "intersection",
            "street_names": []
        })
    
    # Procesar aristas
    for u, v, data in G.edges(data=True):
        edges.append({
            "from": u,
            "to": v,
            "weight": data.get('length', 0),  # longitud en metros
            "street_name": data.get('name', 'Sin nombre'),
            "street_type": data.get('highway', 'unknown'),
            "one_way": data.get('oneway', False),
            "max_speed": data.get('maxspeed', 50)
        })
    
    # Crear dataset final
    dataset = {
        "description": f"Red de calles de {ciudad} extraída de OpenStreetMap",
        "nodes": nodes,
        "edges": edges,
        "metadata": {
            "city": ciudad,
            "source": "OpenStreetMap via OSMnx",
            "extracted_date": "2024-01-15",
            "total_nodes": len(nodes),
            "total_edges": len(edges),
            "network_type": "drive",
            "simplified": True
        }
    }
    
    # Guardar archivo
    filename = f"calles_{ciudad.replace(',', '_').replace(' ', '_').lower()}.json"
    with open(filename, 'w', encoding='utf-8') as f:
        json.dump(dataset, f, indent=2, ensure_ascii=False)
    
    print(f"💾 Dataset guardado como: {filename}")
    print(f"📊 Estadísticas:")
    print(f"   - Nodos: {len(nodes)}")
    print(f"   - Aristas: {len(edges)}")
    print(f"   - Densidad: {len(edges)/len(nodes):.2f} aristas/nodo")
    
    return dataset

def extraer_por_coordenadas(centro_lat, centro_lon, distancia_km=5):
    """
    Extrae calles por coordenadas específicas
    """
    print(f"🗺️ Extrayendo calles alrededor de ({centro_lat}, {centro_lon})...")
    
    # Crear punto central
    centro = (centro_lat, centro_lon)
    
    # Obtener red de calles
    G = ox.graph_from_point(centro, dist=distancia_km*1000, network_type='drive')
    try:
        G = ox.simplify_graph(G)
    except Exception as e:
        print(f"⚠️ El grafo ya está simplificado: {e}")
        pass
    
    print(f"✅ Red extraída: {len(G.nodes)} nodos, {len(G.edges)} aristas")
    
    # Convertir a formato JSON (mismo proceso que arriba)
    # ... (código similar)
    
    return G

if __name__ == "__main__":
    # Ejemplo de uso
    try:
        # Extraer calles de Culiacán
        dataset = extraer_calles_ciudad("Culiacán, Sinaloa, México", distancia_km=10)
        
        # O extraer por coordenadas específicas
        # dataset = extraer_por_coordenadas(24.7841, -107.3866, distancia_km=5)
        
    except Exception as e:
        print(f"❌ Error: {e}")
        print("💡 Asegúrate de tener instalado: pip install osmnx")
