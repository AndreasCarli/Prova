#include <stdio.h>
#include <string.h>

// Gera a tabela Base64 automaticamente
void gerar_tabela_base64(char *tabela) {
    for (int i = 0; i < 26; i++) {
        tabela[i] = 'A' + i;          
        tabela[26 + i] = 'a' + i;     
        tabela[52 + i] = '0' + i;      
    }
    tabela[62] = '+'; 
    tabela[63] = '/';  
    tabela[64] = '\0'; 
}

// Codifica uma string em Base64
void base64_encode(const char *input, char *output) {
    char tabela[65];
    gerar_tabela_base64(tabela);  // Gera a tabela Base64

    int i = 0, j = 0;
    unsigned char arr[3];
    size_t len = strlen(input);

    while (len--) {
        arr[i++] = *(input++);
        if (i == 3) {
            output[j++] = tabela[arr[0] >> 2];
            output[j++] = tabela[(arr[0] & 0x3) << 4 | (arr[1] >> 4)];
            output[j++] = tabela[(arr[1] & 0xf) << 2 | (arr[2] >> 6)];
            output[j++] = tabela[arr[2] & 0x3f];
            i = 0;
        }
    }

    if (i) {
        for (int k = i; k < 3; k++) arr[k] = 0;
        output[j++] = tabela[arr[0] >> 2];
        output[j++] = tabela[(arr[0] & 0x3) << 4 | (arr[1] >> 4)];
        output[j++] = (i == 1) ? '=' : tabela[(arr[1] & 0xf) << 2 | (arr[2] >> 6)];
        output[j++] = '=';
    }

    output[j] = '\0';
}

// Decodifica uma string em Base64
int base64_decode(const char *input, unsigned char *output) {
    char tabela[65];
    gerar_tabela_base64(tabela);  // Gera a tabela Base64

    int i = 0, j = 0, k;
    unsigned char arr[4];

    while (*input) {
        if (*input == '=') break;

        for (k = 0; k < 64; k++) {
            if (tabela[k] == *input) {
                arr[i++] = k;
                break;
            }
        }

        if (i == 4) {
            output[j++] = (arr[0] << 2) | (arr[1] >> 4);
            output[j++] = (arr[1] << 4) | (arr[2] >> 2);
            output[j++] = (arr[2] << 6) | arr[3];
            i = 0;
        }

        input++;
    }

    if (i) {
        if (i == 2) output[j++] = (arr[0] << 2) | (arr[1] >> 4);
        else if (i == 3) {
            output[j++] = (arr[0] << 2) | (arr[1] >> 4);
            output[j++] = (arr[1] << 4) | (arr[2] >> 2);
        }
    }

    output[j] = '\0';
    return j;
}

// Função de hash simples para verificação de autenticidade
void hash_simples(const char *input, char *output) {
    unsigned int hash = 5381;
    while (*input) {
        hash = ((hash << 5) + hash) ^ *input++; // hash * 33 ^ c
    }
    sprintf(output, "%08x", hash);
}

// Codifica a mensagem com a verificação de autenticidade
void codificar_mensagem(const char *mensagem, char *saida) {
    char pacote[512], hash[9];
    hash_simples(mensagem, hash);
    sprintf(pacote, "%s|%s", mensagem, hash);
    base64_encode(pacote, saida);
}

// Decodifica a mensagem e verifica autenticidade
int decodificar_mensagem(const char *entrada, char *mensagem_decodificada) {
    unsigned char buffer[512];
    base64_decode(entrada, buffer);

    char *delim = strchr((char *)buffer, '|');
    if (!delim) return 0;

    *delim = '\0';
    const char *mensagem = (char *)buffer;
    const char *hash_recebido = delim + 1;

    char hash_calculado[9];
    hash_simples(mensagem, hash_calculado);

    if (strcmp(hash_recebido, hash_calculado) == 0) {
        strcpy(mensagem_decodificada, mensagem);
        return 1;
    }

    return 0;
}

// Função principal
int main() {
    char mensagem[512];

    // Solicita ao usuário para digitar uma mensagem
    printf("Digite uma mensagem para codificar: ");
    fgets(mensagem, sizeof(mensagem), stdin);
    mensagem[strcspn(mensagem, "\n")] = 0; 

    printf("Mensagem original: %s\n", mensagem);

    // Codifica a mensagem
    char codificada[512];
    codificar_mensagem(mensagem, codificada);
    printf("Codificada: %s\n", codificada);

    // Decodifica a mensagem e verifica a autenticidade
    char decodificada[256];
    if (decodificar_mensagem(codificada, decodificada)) {
        printf("Mensagem decodificada e autenticada com sucesso: %s\n", decodificada);
    } else {
        printf("Autenticidade inválida.\n");
    }

    return 0;
}